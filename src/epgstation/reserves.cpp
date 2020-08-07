/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/reserves.h"
#include "epgstation/api.h"
#include "epgstation/recorded.h"
#include "epgstation/schedule.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"
#include <map>
#include <string>

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Reserve::refresh()
{
    nlohmann::json response;
    std::map<ReservedState, int (*)(nlohmann::json & response)> requests = {
        { STATE_RESERVED, api::getReserves },
        { STATE_CONFLICT, api::getReservesConflicts },
        { STATE_SKIPPED, api::getReservesSkips },
    };

    reserves.clear();

    for (const auto request : requests) {
        if (request.second(response) == api::REQUEST_FAILED) {
            return false;
        }

        for (const auto& r : response["reserves"]) {
            auto p = r["program"].get<program>();
            p.ruleId = r.contains("ruleId") && r["ruleId"].is_number() ? r["ruleId"].get<int16_t>() : -1;
            p.state = request.first;
            reserves.push_back(p);
        }
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated conflicted program: ammount = %d", response["reserves"].size());
    return true;
}

bool Reserve::add(const std::string id)
{
    const auto success = api::postReserves(id) != api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR,
        "Program reservation: #%s", id.c_str());
    return success;
}

bool Reserve::remove(const std::string id)
{
    const auto success = api::deleteReserves(id) != api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR,
        "Program deletion: #%s", id.c_str());
    return success;
}

bool Reserve::restore(const std::string id)
{
    const auto success = api::deleteReservesSkip(id) != api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR,
        "Program restoration: #%s", id.c_str());
    return success;
}

} // namespace epgstation
