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

bool Schedule::update()
{
    const auto success = api::putScheduleUpdate() != epgstation::api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR, "Schedule update");
    return success;
}
} // namespace epgstation
