
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
#include <cstdlib>
#include <algorithm>
#include "scenario.h"
#include "common.h"
#include "tune.h"
#include <cstring>
uint64_t g_scenario_buf_cnt = 0;
char *get_version()
{
    return "v1.0";
}

char *get_description()
{
    return "tune_example";
}

char *get_name()
{
    return "tune_example";
}

char *get_dep()
{
    return "scenario_example";
}

int get_cycle()
{
    return 1000;
}

void enable()
{
}
void disable()
{
}

void tune(void *info[], int len)
{

    for (int i = 0; i < len; i++) {
        struct DataHeader *header = (struct DataHeader *)info[i];
        if (strcmp(header->type, SCENARIO_ACCESS_BUF) == 0) {
            int dataNum = std::min(header->count - g_scenario_buf_cnt, (uint64_t)header->buf_len);
            if (dataNum != 1) { // demo only support one data
                return;
            }
            struct DataBuf *buf = nullptr;
            int offset = (header->index + header->buf_len - 0) % header->buf_len;
            buf = &header->buf[offset];
            Tune &ins = Tune::getInstance();
            ins.read_access_data((struct uncore_scenario *)buf->data, buf->len);
            ins.migration();
            g_scenario_buf_cnt = header->count;
        }
    }
}


struct TuneInterface tune_interface = {
    .get_version = get_version,
    .get_name = get_name,
    .get_description = get_description,
    .get_dep = get_dep,
    .get_cycle = get_cycle,
    .enable = enable,
    .disable = disable,
    .tune = tune,
};
extern "C" int get_instance(TuneInterface * *ins)
{
    *ins = &tune_interface;
    return 1;
}

