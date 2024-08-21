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


static struct kprobe kp_bind = {
    .symbol_name = "__sys_bind",
};
static struct kprobe kp_connect = {
    .symbol_name = "__sys_connect"
};


#define SMCPROTO_SMC		0	/* SMC protocol, IPv4 */
#define SMCPROTO_SMC6		1	/* SMC protocol, IPv6 */
#define FDPUT_FPUT          1
// di stand for first arg
// si stand for second arg
// dx stand for third arg

#define AF_SMC		43


static int handler_smc(int ifd) {
    long ret;
    int tmperr;
    struct socket *sock;
    
    struct fd f = fdget(ifd);
    if (!f.file) {
        fdput(f);
        return 0;
    }

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5, 10, 0)
    sock = sock_from_file(f.file, &tmperr);
#else
    sock = sock_from_file(f.file);
#endif

    if (sock) {
        struct sock * sk = sock->sk;
        if (sk) {
            u16 protocol = sk->sk_protocol;
            u16 family = sk->sk_family;
            u16 type = sk->sk_type;
            if ((family == AF_INET || family == AF_INET6) &&
                ((type & 0xf) == SOCK_STREAM ) &&
                (protocol == IPPROTO_TCP || protocol == IPPROTO_IP)) {
                    ret = tcp_setsockopt(sk, SOL_TCP, TCP_ULP, KERNEL_SOCKPTR("smc"), sizeof("smc"));
                    if (ret) {
                        printk(KERN_INFO "kprobe: bind or listen failed to set smc failed error id : %ld\n", ret);
                    }
            }
        }

    }
    fdput(f);
    return 0;
}


static int __kprobes handler_bind_pre(struct kprobe *p, struct pt_regs *regs)
{
    int ifd;
#ifdef __x86_64__
    ifd = (int)((regs)->di);
#elif defined(__aarch64__)
    ifd = (int)((regs)->regs[0]);
#endif
    if (!p) {
        return 0;
    }
    
    handler_smc(ifd);
    return 0;
}



static int __kprobes handler_connect_pre(struct kprobe *p, struct pt_regs *regs)
{
    int ifd;
#ifdef __x86_64__
    ifd = (int)((regs)->di);
#elif defined(__aarch64__)
    ifd = (int)((regs)->regs[0]);
#endif
    if (!p) {
        return 0;
    }

    handler_smc(ifd);
    return 0;
}

static int __init kprobe_init(void)
{
    kp_bind.pre_handler = handler_bind_pre;
    kp_connect.pre_handler = handler_connect_pre;
    register_kprobe(&kp_bind);

    register_kprobe(&kp_connect);
    printk(KERN_INFO "smc_acc : module loaded\n");
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp_bind);
    unregister_kprobe(&kp_connect);

    printk(KERN_INFO "smc_acc : module unloaded\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");
