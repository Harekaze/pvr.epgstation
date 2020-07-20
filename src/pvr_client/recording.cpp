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
#include <climits>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

extern epgstation::Recorded g_recorded;
extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

extern "C" {

int GetRecordingsAmount(bool deleted)
{
    return g_recorded.programs.size();
}

PVR_ERROR GetRecordings(ADDON_HANDLE handle, bool deleted)
{
    if (g_recorded.refresh()) {
        for (const auto r : g_recorded.programs) {
            const auto genre = epgstation::getGenreCodeFromContentNibble(r.genre1, r.genre2);
            PVR_RECORDING rec;
            strncpy(rec.strRecordingId, std::to_string(r.id).c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(rec.strTitle, r.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(rec.strPlotOutline, r.description.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(rec.strPlot, r.extended.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            rec.recordingTime = r.startAt;
            rec.iDuration = (int)(r.endAt - r.startAt);
            rec.iGenreType = genre & epgstation::GENRE_TYPE_MASK;
            rec.iGenreSubType = genre & epgstation::GENRE_SUBTYPE_MASK;
            rec.iEpgEventId = r.programId;
            rec.iChannelUid = r.channelId;
            rec.channelType = PVR_RECORDING_CHANNEL_TYPE_TV;
            if (r.hasThumbnail) {
                snprintf(rec.strThumbnailPath, PVR_ADDON_URL_STRING_LENGTH - 1, (const char*)(epgstation::api::baseURL + g_recorded.recordedThumbnailPath).c_str(), rec.strRecordingId);
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

            PVR->TransferRecordingEntry(handle, &rec);
        }
        return PVR_ERROR_NO_ERROR;
    }
    return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetRecordingStreamProperties(const PVR_RECORDING* recording, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
    strncpy(properties[0].strName, PVR_STREAM_PROPERTY_STREAMURL, sizeof(properties[0].strName) - 1);
    snprintf(properties[0].strValue, sizeof(properties[0].strValue) - 1, (const char*)(epgstation::api::baseURL + g_recorded.recordedStreamingPath).c_str(), recording->strRecordingId);
    *iPropertiesCount = 1;
    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING& recording)
{
    if (epgstation::api::deleteRecordedProgram(recording.strRecordingId) != epgstation::api::REQUEST_FAILED) {
        XBMC->Log(ADDON::LOG_NOTICE, "Delete recording: %s", recording.strRecordingId);
        sleep(1);
        PVR->TriggerRecordingUpdate();
        return PVR_ERROR_NO_ERROR;
    }
    return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetDriveSpace(long long* iTotal, long long* iUsed)
{
    const time_t refreshInterval = 10 * 60; // every 10 minutes
    static time_t lastUpdated;
    static long long total, used;
    PVR_ERROR ret;

    time_t now;
    time(&now);

    if (now - lastUpdated < refreshInterval) {
        *iTotal = total;
        *iUsed = used;
        return PVR_ERROR_NO_ERROR;
    }

    ret = epgstation::Storage::getStorageInfo(&used, &total);
    if (ret != PVR_ERROR_NO_ERROR) {
        return ret;
    }

    *iTotal = total;
    *iUsed = used;

    lastUpdated = now;
    return PVR_ERROR_NO_ERROR;
}

/* not implemented */
PVR_ERROR UndeleteRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteAllRecordingsFromTrash(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLifetime(const PVR_RECORDING* recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingPlayCount(const PVR_RECORDING& recording, int count) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING& recording, int lastplayedposition) { return PVR_ERROR_NOT_IMPLEMENTED; }
int GetRecordingLastPlayedPosition(const PVR_RECORDING& recording) { return -1; }
PVR_ERROR GetRecordingEdl(const PVR_RECORDING&, PVR_EDL_ENTRY[], int*) { return PVR_ERROR_NOT_IMPLEMENTED; };
bool OpenRecordedStream(const PVR_RECORDING& recording) { return false; }
void CloseRecordedStream(void) {}
int ReadRecordedStream(unsigned char* pBuffer, unsigned int iBufferSize) { return 0; }
long long SeekRecordedStream(long long iPosition, int iWhence) { return 0; }
long long LengthRecordedStream(void) { return 0; }
}
