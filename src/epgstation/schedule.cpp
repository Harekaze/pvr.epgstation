/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "schedule.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Schedule::refresh()
{
    picojson::value response;
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

    for (picojson::value& a : response.get<picojson::array>()) {
        picojson::object& o = a.get<picojson::object>();
        if (o["programs"].get<picojson::array>().empty()) {
            continue;
        }

        picojson::object& c = o["channel"].get<picojson::object>();

        PVR_CHANNEL ch;
        ch.iUniqueId = c["id"].is<double>() ? (unsigned int)(c["id"].get<double>()) : 0;
        ch.bIsRadio = false;
        ch.bIsHidden = false;
        ch.iChannelNumber = c["remoteControlKeyId"].is<double>() ? (int)((c["remoteControlKeyId"].get<double>())) : 0;
        ch.iSubChannelNumber = c["networkId"].is<double>() ? (unsigned int)(c["networkId"].get<double>()) : 0;
        strncpy(ch.strChannelName, c["name"].is<std::string>() ? c["name"].get<std::string>().c_str() : "", PVR_ADDON_NAME_STRING_LENGTH - 1);

        if (c["hasLogoData"].is<bool>() && c["hasLogoData"].get<bool>()) {
            snprintf(ch.strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1,
                (const char*)(epgstation::api::baseURL + channelLogoPath).c_str(),
                (unsigned int)(c["id"].get<double>()));
        } else {
            ch.strIconPath[0] = '\0';
        }

        const std::string strChannelType = c["channelType"].get<std::string>();
        channelGroups[strChannelType].push_back(ch);

        for (picojson::value& ps : o["programs"].get<picojson::array>()) {
            picojson::object& p = ps.get<picojson::object>();
            struct EPG_PROGRAM epg;
            epg.startTime = (time_t)(p["startAt"].get<double>() / 1000);
            epg.endTime = (time_t)(p["endAt"].get<double>() / 1000);
            epg.strUniqueBroadcastId = std::to_string((unsigned int)p["id"].get<double>());
            epg.iUniqueBroadcastId = (unsigned int)p["id"].get<double>();
            epg.strTitle = p["name"].is<std::string>() ? p["name"].get<std::string>() : "";
            epg.strEpisodeName = ""; // FIXME: Specify a name of episode
            epg.strPlotOutline = p["description"].is<std::string>() ? p["description"].get<std::string>() : "";
            epg.strPlot = p["extended"].is<std::string>() ? p["extended"].get<std::string>() : "";
            epg.strOriginalTitle = epg.strTitle;
            epg.strGenreDescription = p["genre1"].is<std::string>() ? p["genre1"].get<std::string>() : "";
            epg.iEpisodeNumber = 0; // FIXME: Specify a number of episode

            schedule[ch.iUniqueId].push_back(epg);
        }
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", schedule.size());

    lastUpdated = now;
    return true;
}
}
