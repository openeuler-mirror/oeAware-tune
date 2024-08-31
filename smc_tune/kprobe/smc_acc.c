/******************************************************************************
 * Copyright (c) 2024 Huawei Technologies Co., Ltd. All rights reserved.
 * oeAware is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 ******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <net/sock.h>
#include <net/tcp.h>
#include <linux/version.h>
#include <linux/sockptr.h>
#include <linux/net.h>

static struct kprobe kp_sc = {
    .symbol_name = "__sock_create",
};

#ifdef __x86_64__
#define REGS_PARM1(x) ((x)->di)
#define REGS_PARM2(x) ((x)->si)
#define REGS_PARM3(x) ((x)->dx)
#define REGS_PARM4(x) ((x)->cx)
#define REGS_PARM5(x) ((x)->r8)
#define REGS_PARM6(x) ((x)->r9)
#elif defined(__aarch64__)
#define REGS_PARM1(x) ((x)->regs[0])
#define REGS_PARM2(x) ((x)->regs[1])
#define REGS_PARM3(x) ((x)->regs[2])
#define REGS_PARM4(x) ((x)->regs[3])
#define REGS_PARM5(x) ((x)->regs[4])
#define REGS_PARM6(x) ((x)->regs[5])
#endif

#define SMCPROTO_SMC 0  /* SMC protocol, IPv4 */
#define SMCPROTO_SMC6 1 /* SMC protocol, IPv6 */
#define FDPUT_FPUT 1

#define AF_SMC 43

#define SMC_LOADED 0
#define SMC_UNLOADED 1
static int is_smc_loaded = SMC_LOADED;

static unsigned long (*kallsyms_lookup_name_sym)(const char *name);
static int _kallsyms_lookup_kprobe(struct kprobe *p, struct pt_regs *regs)
{
    return 0;
}

unsigned long get_kallsyms_func(char *func_name)
{
    struct kprobe probe;
    int ret;
    unsigned long addr;

    memset(&probe, 0, sizeof(probe));
    probe.pre_handler = _kallsyms_lookup_kprobe;
    probe.symbol_name = func_name;
    ret               = register_kprobe(&probe);
    if (ret)
        return 0;
    addr = (unsigned long)probe.addr;
    unregister_kprobe(&probe);
    return addr;
}

unsigned long generic_kallsyms_lookup_name(const char *name)
{
    if (!kallsyms_lookup_name_sym) {
        kallsyms_lookup_name_sym = (void *)get_kallsyms_func("kallsyms_lookup_name");
        if (!kallsyms_lookup_name_sym)
            return 0;
    }
    return kallsyms_lookup_name_sym(name);
}

static int __init check_smc_module(void)
{
    if (generic_kallsyms_lookup_name("smc_ism_init") == 0) {
        printk(KERN_ERR "SMC module is not loaded.\n");
        return 1;
    }
    return 0;
}

static int __kprobes handler_sk_create_pre(struct kprobe *p, struct pt_regs *regs)
{
    int kern     = (int)REGS_PARM6(regs);
    int family   = (int)REGS_PARM2(regs);
    int type     = (int)REGS_PARM3(regs);
    int protocol = (int)REGS_PARM4(regs);

    if (!kern && (family == AF_INET || family == AF_INET6) && type == SOCK_STREAM &&
        (protocol == IPPROTO_IP || protocol == IPPROTO_TCP)) {
        REGS_PARM4(regs) = (family == AF_INET) ? SMCPROTO_SMC : SMCPROTO_SMC6;
        REGS_PARM2(regs) = AF_SMC;
    }

    return 0;
}

static int __init kprobe_init(void)
{
    if (check_smc_module() != 0) {
        is_smc_loaded = SMC_UNLOADED;
        return -ENOENT;
    }
    kp_sc.pre_handler = handler_sk_create_pre;

    register_kprobe(&kp_sc);

    printk(KERN_INFO "smc_acc : module loaded\n");
    return 0;
}

static void __exit kprobe_exit(void)
{
    if (is_smc_loaded == SMC_UNLOADED)
        return;

    unregister_kprobe(&kp_sc);
    printk(KERN_INFO "smc_acc : module unloaded\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");
