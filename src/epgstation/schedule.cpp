/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/schedule.h"
#include "epgstation/api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"
#include <algorithm>
#include <string>
#include <time.h>
#include <vector>

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
std::vector<program> Schedule::fetch(uint32_t channelId, time_t start, time_t end)
{
    nlohmann::json response;

    list[channelId] = std::vector<program>();

#if defined(_WIN32) || defined(_WIN64)
    const auto t = localtime(&start);
#else
    auto t = new struct tm;
    localtime_r(&start, t);
#endif

    char time[10] = { 0 };
    strftime(time, sizeof(time) - 1, "%y%m%d%H", t);
    const auto days = static_cast<uint16_t>(std::ceil(difftime(end, start) / 86400));
#if !defined(_WIN32) && !defined(_WIN64)
    delete t;
#endif

    if (api::getSchedule(std::to_string(channelId), time, days, response) == api::REQUEST_FAILED) {
        return list[channelId];
    }

    for (const auto& o : response) {
        std::copy(o["programs"].begin(), o["programs"].end(), std::back_inserter(list[channelId]));
    }
    return list[channelId];
}

bool Schedule::update()
{
    const auto success = api::putScheduleUpdate() != epgstation::api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR, "Schedule update");
    return success;
}
} // namespace epgstation
