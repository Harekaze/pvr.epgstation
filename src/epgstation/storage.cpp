/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "storage.h"
#include "api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
PVR_ERROR Storage::getStorageInfo(long long* used, long long* total)
{
    nlohmann::json response;
    if (api::getStorage(response) == api::REQUEST_FAILED) {
        return PVR_ERROR_SERVER_ERROR;
    }

    auto info = response.get<storage>();

    *total = info.total / 1024;
    *used = info.used / 1024;

    return PVR_ERROR_NO_ERROR;
}
}
