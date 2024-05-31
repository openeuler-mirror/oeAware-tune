
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

const char *get_version()
{
    return "v1.0";
}

const char *get_name()
{
    return "tune_example";
}

const char *get_description()
{
    return "tune_example";
}

const char *get_dep()
{
    return "scenario_example";
}

int get_priority()
{
    return 2;
}

int get_period()
{
    return 1000;
}

bool enable()
{
    return true;
}

void disable()
{
}

void run(const struct Param *para)
{

    for (int i = 0; i < para->len; i++) {
        struct DataRingBuf *ringbuf = (struct DataRingBuf *)para->ring_bufs[i];
        if (strcmp(ringbuf->instance_name, SCENARIO_ACCESS_BUF) == 0) {
            int dataNum = std::min(ringbuf->count - g_scenario_buf_cnt, (uint64_t)ringbuf->buf_len);
            if (dataNum != 1) { // demo only support one data
                return;
            }
            struct DataBuf *buf = nullptr;
            int offset = (ringbuf->index + ringbuf->buf_len - 0) % ringbuf->buf_len;
            buf = &ringbuf->buf[offset];
            Tune &ins = Tune::getInstance();
            ins.read_access_data((struct uncore_scenario *)buf->data, buf->len);
            ins.migration();
            g_scenario_buf_cnt = ringbuf->count;
        }
    }
}

struct Interface tune_interface = {
    .get_version = get_version,
    .get_name = get_name,
    .get_description = get_description,
    .get_dep = get_dep,
    .get_priority = get_priority,
    .get_type = nullptr,
    .get_period = get_period,
    .enable = enable,
    .disable = disable,
    .get_ring_buf = nullptr,
    .run = run,
};

extern "C" int get_instance(struct Interface **interface)
{
    *interface = &tune_interface;
    return 1;
}
