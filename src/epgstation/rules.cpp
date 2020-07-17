/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "rules.h"
#include "api.h"
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
    for (nlohmann::json& p : response["rules"]) {
        struct RULE_ITEM rule;
        rule.iIndex = p["id"].is_number() ? (unsigned int)p["id"].get<double>() : i++;
        rule.strTitle = p["keyword"].is_string() ? p["keyword"].get<std::string>() : "";
        rule.strEpgSearchString = rule.strTitle;
        rule.bFullTextEpgSearch = p["description"].is_boolean() && p["description"].get<bool>();
        rule.iClientChannelUid = PVR_TIMER_ANY_CHANNEL; // FIXME: Set valid channel type
        rule.bIsDisabled = !(p["enable"].is_boolean() && p["enable"].get<bool>());
        rule.state = rule.bIsDisabled ? PVR_TIMER_STATE_DISABLED : PVR_TIMER_STATE_SCHEDULED;

        rules.push_back(rule);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated rules: ammount = %d", rules.size());

    return true;
}
}
