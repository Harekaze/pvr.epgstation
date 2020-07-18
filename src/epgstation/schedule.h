/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_SCHEDULE_H
#define EPGSTATION_SCHEDULE_H
#include <iostream>
#include <map>

#include "epgstation/types.h"

namespace epgstation {
class Schedule {
public:
    std::string channelLogoPath;
    std::string liveStreamingPath;
    std::vector<program> programs;
    std::vector<channel> channels;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
