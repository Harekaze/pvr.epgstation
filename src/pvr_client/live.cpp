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
#include <iostream>

extern epgstation::Schedule g_schedule;
extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

extern "C" {

PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL& channel, time_t iStart, time_t iEnd)
{
    for (const auto epg : g_schedule.programs) {
        if (epg.channelId != channel.iUniqueId)
            continue;
        if (epg.endAt < iStart)
            continue;
        if (epg.startAt > iEnd)
            break;

        EPG_TAG tag;
        memset(&tag, 0, sizeof(EPG_TAG));
        const auto genre = epgstation::getGenreCodeFromContentNibble(epg.genre1, epg.genre2);

        tag.iUniqueBroadcastId = static_cast<unsigned int>(epg.id % 100000);
        tag.strTitle = epg.name.c_str();
        tag.strOriginalTitle = epg.name.c_str();
        tag.iUniqueChannelId = channel.iUniqueId;
        tag.startTime = epg.startAt;
        tag.endTime = epg.endAt;
        tag.strPlotOutline = epg.description.c_str();
        tag.strPlot = epg.extended.c_str();
        tag.iGenreType = genre & epgstation::GENRE_TYPE_MASK;
        tag.iGenreSubType = genre & epgstation::GENRE_SUBTYPE_MASK;

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
    char url[1024];
    snprintf(url, sizeof(url) - 1, g_schedule.liveStreamingPath.c_str(), channel.iUniqueId);
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
PVR_ERROR GetEPGTagEdl(const EPG_TAG* epgTag, PVR_EDL_ENTRY edl[], int* size) { return PVR_ERROR_NOT_IMPLEMENTED; }
long long SeekLiveStream(long long iPosition, int iWhence) { return -1; }
long long LengthLiveStream(void) { return -1; }
}
