/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/channels.h"
#include "epgstation/api.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"
#include <algorithm>
#include <string>
#include <vector>

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Channels::refresh()
{
    nlohmann::json response;

    channels.clear();

    if (api::getChannels(response) == api::REQUEST_FAILED) {
        return false;
    }

    std::copy(response.begin(), response.end(), std::back_inserter(channels));

    XBMC->Log(ADDON::LOG_NOTICE, "Updated channels: channel ammount = %d", channels.size());
    return true;
}
} // namespace epgstation
