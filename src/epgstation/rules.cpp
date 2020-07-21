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
    rules.clear();

    if (api::getRules(response) == api::REQUEST_FAILED) {
        return false;
    }

    for (const rule& r : response["rules"]) {
        rules.push_back(r);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated rules: ammount = %d", rules.size());
    return true;
}

bool Rule::add(bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory)
{
    if (api::postRules(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule created: \"%s\"", searchText.c_str());
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to create rule: \"%s\"", searchText.c_str());
    return false;
}

bool Rule::edit(int id, bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory)
{
    if (api::putRule(id, enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule updated: #%d", id);
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to update rule: #%d", id);
    return false;
}

bool Rule::enable(int id, bool enabled)
{
    if (api::putRuleAction(id, enabled) != api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Rule %s: #%d", enabled ? "enabled" : "disabled", id);
        return true;
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to %s rule: #%d", enabled ? "enable" : "disable", id);
    return false;
}

}
