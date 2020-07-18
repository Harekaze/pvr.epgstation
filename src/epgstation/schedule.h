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
#include "kodi/xbmc_epg_types.h"
#include "kodi/xbmc_pvr_types.h"

namespace epgstation {
class Schedule {
public:
    std::string channelLogoPath;
    std::string liveStreamingPath;
    std::map<unsigned int, std::vector<program>> schedule;
    std::map<std::string, std::vector<channel>> channelGroups;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
