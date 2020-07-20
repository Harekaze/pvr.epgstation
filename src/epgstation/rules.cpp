/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "rules.h"
#include "api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "recorded.h"
#include "reserves.h"
#include "schedule.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Rule::refresh()
{
    nlohmann::json response;

    if (epgstation::api::getRules(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    rules.clear();

    unsigned int i = 0;
    for (const auto& p : response["rules"]) {
        auto rule = p.get<epgstation::rule>();
        rules.push_back(rule);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated rules: ammount = %d", rules.size());

    return true;
}
}
