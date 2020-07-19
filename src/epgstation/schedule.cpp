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

    programs.clear();
    channels.clear();

    std::vector<std::string> types = { "GR", "BS", "CS" };
    for (const auto type : types) {
        if (epgstation::api::getSchedule(type, response) == epgstation::api::REQUEST_FAILED) {
            return false;
        }

        for (const auto& o : response) {
            if (o["programs"].empty()) {
                continue;
            }
            epgstation::channel ch = o["channel"].get<epgstation::channel>();
            channels.push_back(ch);

            for (const auto& pp : o["programs"]) {
                auto p = pp.get<epgstation::program>();
                programs.push_back(p);
            }
        }
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", 8);

    lastUpdated = now;
    return true;
}
}
