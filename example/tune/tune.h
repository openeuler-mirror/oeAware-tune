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
#ifndef __TUNE_HH__
#define __TUNE_HH__

#include "scenario.h"


class Tune {
public:
    static Tune &getInstance()
    {
        static Tune instance;
        return instance;
    }
    Tune(const Tune &) = delete;
    Tune &operator=(const Tune &) = delete;
    Tune();
    ~Tune() {}

private:
    float numa_score;


public:
    void read_access_data(struct uncore_scenario *data, int dateLen);
    void migration();
};

#endif