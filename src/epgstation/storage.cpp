/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "storage.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
PVR_ERROR Storage::getStorageInfo(long long* used, long long* total)
{
    nlohmann::json response;
    if (epgstation::api::getStorage(response) == epgstation::api::REQUEST_FAILED) {
        return PVR_ERROR_SERVER_ERROR;
    }

    *total = (long long)(response["total"].get<double>() / 1024);
    *used = (long long)(response["used"].get<double>() / 1024);

    return PVR_ERROR_NO_ERROR;
}
}
