/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/epgstation.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include <iostream>

extern epgstation::Schedule g_schedule;
extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

extern "C" {

PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL& channel, time_t iStart, time_t iEnd)
{
    for (const epgstation::EPG_PROGRAM epg : g_schedule.schedule[channel.iUniqueId]) {
        if (epg.endTime < iStart)
            continue;
        if (epg.startTime > iEnd)
            break;

        EPG_TAG tag;
        memset(&tag, 0, sizeof(EPG_TAG));

        tag.iUniqueBroadcastId = epg.iUniqueBroadcastId;
        tag.strTitle = epg.strTitle.c_str();
        tag.strOriginalTitle = epg.strOriginalTitle.c_str();
        tag.iUniqueChannelId = channel.iUniqueId;
        tag.startTime = epg.startTime;
        tag.endTime = epg.endTime;
        tag.strPlotOutline = epg.strPlotOutline.c_str();
        tag.strPlot = epg.strPlot.c_str();
        tag.iGenreType = epgstation::iGenreTypePair[epg.strGenreDescription] & epgstation::GENRE_TYPE_MASK;
        tag.iGenreSubType = epgstation::iGenreTypePair[epg.strGenreDescription] & epgstation::GENRE_SUBTYPE_MASK;
        tag.iEpisodeNumber = epg.iEpisodeNumber;
        tag.strEpisodeName = epg.strEpisodeName.c_str();
        tag.strGenreDescription = epg.strGenreDescription.c_str();

        PVR->TransferEpgEntry(handle, &tag);
    }

    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IsEPGTagRecordable(const EPG_TAG* tag, bool* bIsRecordable)
{
    *bIsRecordable = true;
    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IsEPGTagPlayable(const EPG_TAG* tag, bool* bIsPlayable)
{
    *bIsPlayable = true;
    return PVR_ERROR_NO_ERROR;
}

void* liveStreamHandle = NULL;
bool OpenLiveStream(const PVR_CHANNEL& channel)
{
    std::string sId = "";
    for (unsigned long iId = channel.iSubChannelNumber * 100000 + channel.iUniqueId; iId > 0; iId /= 36) {
        const int i = iId % 36;
        const char c = i < 10 ? '0' + i : 'a' + i - 10;
        sId = c + sId;
    }
    char url[1024];
    snprintf(url, sizeof(url) - 1, (const char*)(epgstation::api::baseURL + g_schedule.liveStreamingPath).c_str(), sId.c_str());
    liveStreamHandle = XBMC->OpenFile(url, 0);
    return liveStreamHandle != NULL;
}

void CloseLiveStream(void)
{
    if (liveStreamHandle != NULL)
        XBMC->CloseFile(liveStreamHandle);
    liveStreamHandle = NULL;
}

int ReadLiveStream(unsigned char* pBuffer, unsigned int iBufferSize)
{
    return XBMC->ReadFile(liveStreamHandle, pBuffer, iBufferSize);
}

bool IsRealTimeStream()
{
    return true;
}

/* not implemented */
PVR_ERROR GetEPGTagStreamProperties(const EPG_TAG* tag, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR GetEPGTagEdl(const EPG_TAG* epgTag, PVR_EDL_ENTRY edl[], int* size) { return PVR_ERROR_NOT_IMPLEMENTED; };
long long SeekLiveStream(long long iPosition, int iWhence) { return -1; }
long long LengthLiveStream(void) { return -1; }
}
