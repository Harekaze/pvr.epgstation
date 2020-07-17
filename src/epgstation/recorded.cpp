/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "recorded.h"
#include "api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "schedule.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Recorded::refresh()
{
    nlohmann::json response;

    if (epgstation::api::getRecorded(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    const bool showThumbnail = !recordedThumbnailPath.empty();
    programs.clear();

    for (nlohmann::json& p : response["recorded"]) {
        PVR_RECORDING rec;
        char* endptr;
        epgstation::recorded r = p.get<epgstation::recorded>();

        strncpy(rec.strRecordingId, std::to_string(r.id).c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strTitle, r.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strPlotOutline, r.description.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
        strncpy(rec.strPlot, r.extended.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
        rec.recordingTime = r.startAt;
        rec.iDuration = (int)(r.endAt - r.startAt);
        rec.iGenreType = r.genre1;
        rec.iGenreSubType = r.genre2;
        rec.iEpgEventId = r.programId;
        rec.iChannelUid = r.channelId;
        rec.channelType = PVR_RECORDING_CHANNEL_TYPE_TV;
        if (showThumbnail && r.hasThumbnail) {
            snprintf(rec.strThumbnailPath, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(epgstation::api::baseURL + recordedThumbnailPath).c_str(), rec.strRecordingId);
        } else {
            strncpy(rec.strThumbnailPath, "", PVR_ADDON_URL_STRING_LENGTH - 1);
        }

        // Not available in API response
        strncpy(rec.strDirectory, "", PVR_ADDON_URL_STRING_LENGTH - 1);
        strncpy(rec.strEpisodeName, "", PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strChannelName, "", PVR_ADDON_NAME_STRING_LENGTH - 1);
        rec.iEpisodeNumber = 0;
        rec.iPriority = 0;
        rec.bIsDeleted = false;

        programs.push_back(rec);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated recorded program: ammount = %d", programs.size());

    return true;
}
}
