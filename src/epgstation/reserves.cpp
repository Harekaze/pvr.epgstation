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

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Reserve::refresh()
{
    picojson::value response;

    if (epgstation::api::getReserves(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    reserves.clear();

    time_t now;
    time(&now);

    for (picojson::value& a : response.get<picojson::array>()) {
        picojson::object& p = a.get<picojson::object>();
        // Skip past tv program
        if ((p["end"].get<double>() / 1000) < now) {
            continue;
        }
        struct PVR_TIMER resv;
        char* endptr;

        const std::string strSubstrId = p["id"].get<std::string>().substr(p["id"].get<std::string>().size() - 6, 6);
        resv.iEpgUid = strtoul(strSubstrId.c_str(), &endptr, 36);
        resv.iClientIndex = resv.iEpgUid;
        resv.iClientChannelUid = p["channel"].get<picojson::object>()["sid"].is<std::string>() ? std::atoi((p["channel"].get<picojson::object>()["sid"].get<std::string>()).c_str()) : (int)(p["channel"].get<picojson::object>()["sid"].get<double>());
        strncpy(resv.strTitle, p["fullTitle"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(resv.strSummary, p["detail"].get<std::string>().c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
        strncpy(resv.strDirectory, p["id"].get<std::string>().c_str(), PVR_ADDON_URL_STRING_LENGTH - 1); // instead of strProgramId
        if (p["isConflict"].is<bool>() && p["isConflict"].get<bool>()) {
            resv.state = PVR_TIMER_STATE_CONFLICT_NOK;
        } else if (p["isSkip"].is<bool>() && p["isSkip"].get<bool>()) {
            resv.state = PVR_TIMER_STATE_DISABLED;
        } else {
            resv.state = PVR_TIMER_STATE_SCHEDULED;
        }
        resv.startTime = (time_t)(p["start"].get<double>() / 1000);
        resv.endTime = (time_t)(p["end"].get<double>() / 1000);
        resv.iGenreType = epgstation::iGenreTypePair[p["category"].get<std::string>()] & epgstation::GENRE_TYPE_MASK;
        resv.iGenreSubType = epgstation::iGenreTypePair[p["category"].get<std::string>()] & epgstation::GENRE_SUBTYPE_MASK;
        resv.bStartAnyTime = false;
        resv.bEndAnyTime = false;
        resv.iTimerType = (p["isManualReserved"].is<bool>() && p["isManualReserved"].get<bool>()) ? TIMER_MANUAL_RESERVED : TIMER_PATTERN_MATCHED;

        reserves.push_back(resv);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated reserved program: ammount = %d", reserves.size());

    return true;
}
}
