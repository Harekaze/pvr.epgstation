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
    std::map<uint32_t, std::vector<program>> list;
    std::vector<program> fetch(uint32_t channelId, time_t start, time_t end);
    bool update();
};
} // namespace epgstation

#endif // SRC_EPGSTATION_SCHEDULE_H_
