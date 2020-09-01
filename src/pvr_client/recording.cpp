/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/epgstation.h"
#include "epgstation/genre.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include <algorithm>
#include <climits>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

extern epgstation::Recorded g_recorded;
extern epgstation::Channels g_channels;
extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;
extern CHelper_libKODI_guilib* GUI;

extern "C" {

int GetRecordingsAmount(bool deleted)
{
    return g_recorded.programs.size();
}

PVR_ERROR GetRecordings(ADDON_HANDLE handle, bool deleted)
{
    if (g_recorded.refresh()) {
        for (const auto& r : g_recorded.programs) {
            const auto genre = epgstation::getGenreCodeFromContentNibble(r.genre1, r.genre2);
            PVR_RECORDING rec = {};
            rec.recordingTime = r.startAt;
            rec.iDuration = static_cast<int>(r.endAt - r.startAt);
            rec.iGenreType = genre.main;
            rec.iGenreSubType = genre.sub;
            rec.iEpgEventId = static_cast<unsigned int>(r.programId % 100000);
            rec.iChannelUid = g_channels.getId(r.channelId);
            rec.channelType = PVR_RECORDING_CHANNEL_TYPE_TV;
            strncpy(rec.strRecordingId, std::to_string(r.id).c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(rec.strTitle, r.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(rec.strPlotOutline, r.description.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(rec.strPlot, r.extended.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            if (r.hasThumbnail) {
                snprintf(rec.strThumbnailPath, PVR_ADDON_URL_STRING_LENGTH - 1, g_recorded.recordedThumbnailPath.c_str(), rec.strRecordingId);
            }

            PVR->TransferRecordingEntry(handle, &rec);
        }
        return PVR_ERROR_NO_ERROR;
    }
    return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetRecordingStreamProperties(const PVR_RECORDING* recording, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
    const auto rec = std::find_if(g_recorded.programs.begin(), g_recorded.programs.end(), [recording](epgstation::program p) {
        return p.id == std::stoull(recording->strRecordingId);
    });

    if (rec == g_recorded.programs.end()) {
        XBMC->Log(ADDON::LOG_ERROR, "Recording not found: #%s", recording->strRecordingId);
        return PVR_ERROR_FAILED;
    }

    strncpy(properties[0].strName, PVR_STREAM_PROPERTY_STREAMURL, sizeof(properties[0].strName) - 1);

    int prefer_encoded;
    XBMC->GetSetting("prefer_encoded", &prefer_encoded);
    const auto ignoreOriginalPlayback = !rec->encoded.empty() && prefer_encoded;
    if (!rec->original || ignoreOriginalPlayback) {
        auto id = rec->encoded[0].first;

        if (rec->encoded.size() > 1) {
            std::vector<const char*> entries;
            std::transform(rec->encoded.begin(), rec->encoded.end(), std::back_inserter(entries), [](std::pair<uint64_t, std::string>& s) {
                return s.second.c_str();
            });
            entries.push_back(nullptr);

            const auto selected = GUI->Dialog_Select("Select media", entries.data(), rec->encoded.size());
            if (selected < 0) {
                return PVR_ERROR_NOT_IMPLEMENTED;
            }
            id = rec->encoded[selected].first;
        }

        const auto param = "?encodedId=" + std::to_string(id);
        snprintf(properties[0].strValue, sizeof(properties[0].strValue) - 1, (g_recorded.recordedStreamingPath + param).c_str(), recording->strRecordingId);
    } else {
        snprintf(properties[0].strValue, sizeof(properties[0].strValue) - 1, g_recorded.recordedStreamingPath.c_str(), recording->strRecordingId);
    }

    *iPropertiesCount = 1;
    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING& recording)
{
    if (g_recorded.remove(recording.strRecordingId)) {
        sleep(1);
        PVR->TriggerRecordingUpdate();
        return PVR_ERROR_NO_ERROR;
    }
    return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetDriveSpace(long long* iTotal, long long* iUsed)
{
    return epgstation::Storage::getStorageInfo(iUsed, iTotal);
}

/* not implemented */
PVR_ERROR UndeleteRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteAllRecordingsFromTrash(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLifetime(const PVR_RECORDING* recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingPlayCount(const PVR_RECORDING& recording, int count) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING& recording, int lastplayedposition) { return PVR_ERROR_NOT_IMPLEMENTED; }
int GetRecordingLastPlayedPosition(const PVR_RECORDING& recording) { return -1; }
PVR_ERROR GetRecordingEdl(const PVR_RECORDING&, PVR_EDL_ENTRY[], int*) { return PVR_ERROR_NOT_IMPLEMENTED; }
bool OpenRecordedStream(const PVR_RECORDING& recording) { return false; }
void CloseRecordedStream(void) {}
int ReadRecordedStream(unsigned char* pBuffer, unsigned int iBufferSize) { return 0; }
long long SeekRecordedStream(long long iPosition, int iWhence) { return 0; }
long long LengthRecordedStream(void) { return 0; }
}
