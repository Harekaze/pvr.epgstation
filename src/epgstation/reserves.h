/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_RESERVES_H
#define EPGSTATION_RESERVES_H
#include <iostream>

#include "kodi/xbmc_pvr_types.h"

#define TIMER_MANUAL_RESERVED 0x01
#define TIMER_PATTERN_MATCHED 0x02

namespace epgstation {
class Reserve {
public:
    std::vector<PVR_TIMER> reserves;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
