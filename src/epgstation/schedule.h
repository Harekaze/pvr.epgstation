/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_SCHEDULE_H
#define EPGSTATION_SCHEDULE_H
#include <iostream>
#include <map>

#include "epgstation/genre.h"
#include "kodi/xbmc_epg_types.h"
#include "kodi/xbmc_pvr_types.h"

namespace epgstation {
struct EPG_PROGRAM {
    unsigned int iUniqueBroadcastId;
    unsigned int iEpisodeNumber;
    time_t startTime;
    time_t endTime;
    std::string strUniqueBroadcastId;
    std::string strTitle;
    std::string strPlotOutline;
    std::string strPlot;
    std::string strOriginalTitle;
    std::string strEpisodeName;
    std::string strGenreDescription;
};
class Schedule {
public:
    std::string channelLogoPath;
    std::string liveStreamingPath;
    std::map<unsigned int, std::vector<EPG_PROGRAM>> schedule;
    std::map<std::string, std::vector<PVR_CHANNEL>> channelGroups;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
