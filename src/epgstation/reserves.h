/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_RESERVES_H_
#define SRC_EPGSTATION_RESERVES_H_
#include "epgstation/types.h"
#include <iostream>
#include <string>
#include <vector>

#define TIMER_MANUAL_RESERVED 0x01
#define TIMER_PATTERN_MATCHED 0x02

namespace epgstation {
class Reserve {
public:
    std::vector<program> reserves;
    bool refresh();
    bool add(const std::string id);
    bool remove(const std::string id);
    bool restore(const std::string id);
};
} // namespace epgstation

#endif // SRC_EPGSTATION_RESERVES_H_
