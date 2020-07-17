/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "schedule.h"
#include "api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Schedule::refresh()
{
    nlohmann::json response;
    const time_t refreshInterval = 10 * 60; // every 10 minutes
    static time_t lastUpdated;
    time_t now;

    time(&now);
    if (now - lastUpdated < refreshInterval) {
        return true;
    }

    if (epgstation::api::getSchedule(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    schedule.clear();
    channelGroups.clear();

    for (nlohmann::json& o : response) {
        if (o["programs"].empty()) {
            continue;
        }

        epgstation::channel c = o["channel"].get<epgstation::channel>();

        PVR_CHANNEL ch;
        ch.iUniqueId = c.id;
        ch.bIsRadio = false;
        ch.bIsHidden = false;
        ch.iChannelNumber = c.remoteControlKeyId;
        ch.iSubChannelNumber = c.networkId;
        strncpy(ch.strChannelName, c.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);

        if (c.hasLogoData) {
            snprintf(ch.strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1,
                (const char*)(epgstation::api::baseURL + channelLogoPath).c_str(),
                c.id);
        } else {
            ch.strIconPath[0] = '\0';
        }

        channelGroups[c.channelType].push_back(ch);

        for (nlohmann::json& pp : o["programs"]) {
            epgstation::program p = pp.get<epgstation::program>();

            struct EPG_PROGRAM epg;
            epg.startTime = p.startAt;
            epg.endTime = p.endAt;
            epg.strUniqueBroadcastId = std::to_string(p.id);
            epg.iUniqueBroadcastId = p.id;
            epg.strTitle = p.name;
            epg.strEpisodeName = ""; // FIXME: Specify a name of episode
            epg.strPlotOutline = p.description;
            epg.strPlot = p.extended;
            epg.strOriginalTitle = epg.strTitle;
            epg.strGenreDescription = std::to_string(p.genre1) + std::to_string(p.genre2); // FIXME: Valid genre string
            epg.iEpisodeNumber = 0; // FIXME: Specify a number of episode

            schedule[ch.iUniqueId].push_back(epg);
        }
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", schedule.size());

    lastUpdated = now;
    return true;
}
}
