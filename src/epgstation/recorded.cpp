/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "recorded.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"
#include "schedule.h"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Recorded::refresh()
{
    picojson::value response;

    if (epgstation::api::getRecorded(response) == epgstation::api::REQUEST_FAILED) {
        return false;
    }

    const bool showThumbnail = !recordedThumbnailPath.empty();
    programs.clear();

    for (picojson::value& a : response.get<picojson::array>()) {
        picojson::object& p = a.get<picojson::object>();
        PVR_RECORDING rec;
        char* endptr;

        strncpy(rec.strRecordingId, p["id"].is<std::string>() ? p["id"].get<std::string>().c_str() : "", PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strDirectory, p["title"].is<std::string>() ? p["title"].get<std::string>().c_str() : "", PVR_ADDON_URL_STRING_LENGTH - 1);
        strncpy(rec.strTitle, p["title"].is<std::string>() ? p["title"].get<std::string>().c_str() : "", PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strEpisodeName, p["subTitle"].is<std::string>() ? p["subTitle"].get<std::string>().c_str() : "", PVR_ADDON_NAME_STRING_LENGTH - 1);
        strncpy(rec.strPlotOutline, p["description"].is<std::string>() ? p["description"].get<std::string>().c_str() : rec.strEpisodeName, PVR_ADDON_DESC_STRING_LENGTH - 1);
        strncpy(rec.strPlot, p["detail"].is<std::string>() ? p["detail"].get<std::string>().c_str() : "", PVR_ADDON_DESC_STRING_LENGTH - 1);
        strncpy(rec.strChannelName, (p["channel"].get<picojson::object>())["name"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
        rec.iEpisodeNumber = p["episode"].is<double>() ? (unsigned int)(p["episode"].get<double>()) : 0;
        rec.recordingTime = (time_t)(p["start"].get<double>() / 1000);
        rec.iDuration = (int)(p["seconds"].get<double>());
        rec.iPriority = p["priority"].is<double>() ? (int)(p["priority"].get<double>()) : 0;
        const std::string strGenreType = p["category"].get<std::string>();
        rec.iGenreType = epgstation::iGenreTypePair[strGenreType] & epgstation::GENRE_TYPE_MASK;
        rec.iGenreSubType = epgstation::iGenreTypePair[strGenreType] & epgstation::GENRE_SUBTYPE_MASK;
        std::string id = p["id"].get<std::string>();
        std::remove(id.begin(), id.end(), '-');
        const std::string strSubstrId = id.substr(id.size() - 6, 6);
        rec.iEpgEventId = strtoul(strSubstrId.c_str(), &endptr, 36);
        rec.channelType = PVR_RECORDING_CHANNEL_TYPE_TV;
        rec.bIsDeleted = false;
        const int sid = p["channel"].get<picojson::object>()["sid"].is<std::string>() ? std::atoi((p["channel"].get<picojson::object>()["sid"].get<std::string>()).c_str()) : (int)(p["channel"].get<picojson::object>()["sid"].get<double>());
        rec.iChannelUid = sid;
        if (showThumbnail) {
            snprintf(rec.strThumbnailPath, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(epgstation::api::baseURL + recordedThumbnailPath).c_str(), p["id"].get<std::string>().c_str());
        } else {
            strncpy(rec.strThumbnailPath, "", PVR_ADDON_URL_STRING_LENGTH - 1);
        }

        programs.push_back(rec);
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated recorded program: ammount = %d", programs.size());

    return true;
}
}
