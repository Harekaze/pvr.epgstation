/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "reserves.h"
#include "api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "recorded.h"
#include "schedule.h"
#include "json/json.hpp"

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
            p.ruleId = r.contains("ruleId") && r["ruleId"].is_number() ? r["ruleId"].get<int>() : -1;
            p.state = request.first;
            reserves.push_back(p);
        }
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated conflicted program: ammount = %d", response["reserves"].size());
    return true;
}

bool Reserve::add(std::string id)
{
    if (api::postReserves(id) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Reserved new program: #%s", id.c_str());
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to reserve new program: #%s", id.c_str());
    return false;
}

bool Reserve::remove(std::string id)
{
    if (api::deleteReserves(id) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Deleted reserved program: #%s", id.c_str());
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to delete reserved program: #%s", id.c_str());
    return false;
}

bool Reserve::restore(std::string id)
{
    if (api::deleteReservesSkip(id) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Restored program: %s", id.c_str());
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to restore program: %s", id.c_str());
    return false;
}

}
