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

    if (epgstation::api::getReserves(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    reserves.clear();

    for (const auto& r : response["reserves"]) {
        auto p = r["program"].get<epgstation::program>();
        p.ruleId = r["ruleId"].is_number() ? r["ruleId"].get<int>() : -1;
        reserves.push_back(p);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());

    return true;
}
}
