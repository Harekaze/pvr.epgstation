/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include <iostream>

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

char serverUrl[1024];

extern "C" {

PVR_ERROR GetAddonCapabilities(PVR_ADDON_CAPABILITIES* pCapabilities)
{
    pCapabilities->bSupportsEPG = true;
    pCapabilities->bSupportsTV = true;
    pCapabilities->bSupportsRadio = false;
    pCapabilities->bSupportsRecordings = true;
    pCapabilities->bSupportsRecordingsUndelete = false;
    pCapabilities->bSupportsTimers = true;
    pCapabilities->bSupportsChannelGroups = true;
    pCapabilities->bSupportsChannelScan = false;
    pCapabilities->bSupportsChannelSettings = false;
    pCapabilities->bHandlesInputStream = false;
    pCapabilities->bHandlesDemuxing = false;
    pCapabilities->bSupportsRecordingPlayCount = false;
    pCapabilities->bSupportsLastPlayedPosition = false;
    pCapabilities->bSupportsRecordingEdl = false;
    pCapabilities->bSupportsRecordingsRename = false;
    pCapabilities->bSupportsRecordingsLifetimeChange = false;
    pCapabilities->bSupportsDescrambleInfo = false;
    pCapabilities->iRecordingsLifetimesSize = 0;

    return PVR_ERROR_NO_ERROR;
}

const char* GetConnectionString(void)
{
    if (XBMC->GetSetting("server_url", &serverUrl)) {
        return serverUrl;
    }

    return "";
}

const char* GetBackendName(void)
{
    return "EPGStation";
}

const char* GetBackendVersion(void)
{
    return "gamma";
}

const char* GetBackendHostname(void)
{
    return "";
}

/* not implemented */
PVR_ERROR GetDescrambleInfo(PVR_DESCRAMBLE_INFO* descrambleInfo) { return PVR_ERROR_NOT_IMPLEMENTED; }
}
