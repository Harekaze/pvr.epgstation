/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "reserves.h"
#include "api.h"
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

    int index = 0;

    for (nlohmann::json& r : response["reserves"]) {
        nlohmann::json& p = r["program"];

        struct PVR_TIMER resv;

        resv.iEpgUid = (unsigned int)(p["id"].get<double>()); // FIXME: Overflow
        resv.iClientIndex = index++;
        resv.iClientChannelUid = (int)(p["channelId"].get<double>());
        strncpy(resv.strTitle, p["name"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(resv.strSummary, p["extended"].get<std::string>().c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
        strncpy(resv.strDirectory, std::to_string((unsigned long)(p["id"].get<double>())).c_str(), PVR_ADDON_URL_STRING_LENGTH - 1); // NOTE: Store original ID

        resv.state = PVR_TIMER_STATE_SCHEDULED;
        resv.startTime = (time_t)(p["startAt"].get<double>() / 1000);
        resv.endTime = (time_t)(p["endAt"].get<double>() / 1000);
        resv.iGenreType = p["genre1"].is_number() ? (int)(p["genre1"].get<double>()) : 0;
        resv.iGenreSubType = p["genre2"].is_number() ? (int)(p["genre2"].get<double>()) : 0;
        resv.bStartAnyTime = false;
        resv.bEndAnyTime = false;
        resv.iTimerType = r["ruleId"].is_number() ? TIMER_PATTERN_MATCHED : TIMER_MANUAL_RESERVED;

        reserves.push_back(resv);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());

    return true;
}
}
