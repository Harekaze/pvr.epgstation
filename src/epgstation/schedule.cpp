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
#include <vector>

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Schedule::refresh()
{
    nlohmann::json response;

    programs.clear();

    std::vector<std::string> types = { "GR", "BS", "CS" };
    for (const auto type : types) {
        if (api::getScheduleAll(type, response) == api::REQUEST_FAILED) {
            return false;
        }

        for (const auto& o : response) {
            if (o["programs"].empty()) {
                continue;
            }
            std::copy(o["programs"].begin(), o["programs"].end(), std::back_inserter(programs));
        }
    }

    return true;
}

std::vector<program> Schedule::fetch(uint32_t channelId, time_t start, time_t end)
{
    nlohmann::json response;

    list[channelId] = std::vector<program>();

    if (api::getSchedule(std::to_string(channelId), response) == api::REQUEST_FAILED) {
        return list[channelId];
    }

    for (const auto& o : response) {
        if (o["programs"].empty()) {
            continue;
        }
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
