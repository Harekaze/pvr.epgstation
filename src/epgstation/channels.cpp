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

    std::copy_if(response.begin(), response.end(), std::back_inserter(channels), [](epgstation::channel c) {
        return c.type != 0xC0; // filter 1-seg channel
    });

    int id = 1;
    for (auto ch = channels.begin(); ch != channels.end(); ++ch, ++id) {
        ch->internalId = id;
    }

    XBMC->Log(ADDON::LOG_NOTICE, "Updated channels: channel ammount = %d", channels.size());
    return true;
}

int Channels::getId(uint64_t realId)
{
    return std::find_if(channels.begin(), channels.end(), [realId](channel ch) {
        return ch.id == realId;
    })->internalId;
}

uint64_t Channels::getId(int internalId)
{
    return std::find_if(channels.begin(), channels.end(), [internalId](channel ch) {
        return ch.internalId == internalId;
    })->id;
}
} // namespace epgstation
