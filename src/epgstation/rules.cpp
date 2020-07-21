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

bool Rule::add(bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory)
{
    if (epgstation::api::postRules(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory) != epgstation::api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule created: \"%s\"", searchText.c_str());
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to create rule: \"%s\"", searchText.c_str());
    return false;
}

bool Rule::edit(int id, bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory)
{
    if (epgstation::api::putRule(id, enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory) != epgstation::api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule updated: #%d", id);
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to update rule: #%d", id);
    return false;
}

bool Rule::enable(int id, bool enabled)
{
    if (epgstation::api::putRuleAction(id, enabled) != epgstation::api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule %s: #%d", enabled ? "enabled" : "disabled", id);
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to %s rule: #%d", enabled ? "enable" : "disable", id);
    return false;
}

}
