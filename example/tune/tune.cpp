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
#include <iostream>
#include "tune.h"
#include "scenario.h"
Tune::Tune()
{
}

void Tune::read_access_data(struct uncore_scenario *data, int dataLen)
{
    if (dataLen != 1) {
        std::cout << "dataLen != 1" << std::endl;
    } else {
        numa_score = data->numa_score;
    }
}

void Tune::migration()
{
    std::cout << "[tune example] migration:numa_score=" << numa_score << std::endl;

    if (numa_score > 0.2f) {
        std::cout << "[tune example] migration:numa_score > 0.2, start migration" << std::endl;
    }
}