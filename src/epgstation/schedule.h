/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_SCHEDULE_H_
#define SRC_EPGSTATION_SCHEDULE_H_
#include "epgstation/types.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace epgstation {
class Schedule {
public:
    std::vector<program> programs;
    bool refresh();
    bool update();
};
} // namespace epgstation

#endif // SRC_EPGSTATION_SCHEDULE_H_
