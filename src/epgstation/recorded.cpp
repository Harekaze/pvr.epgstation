/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/recorded.h"
#include "epgstation/api.h"
#include "epgstation/schedule.h"
#include "epgstation/types.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"
#include <algorithm>
#include <string>

extern ADDON::CHelper_libXBMC_addon* XBMC;

namespace epgstation {
bool Recorded::refresh()
{
    nlohmann::json response;
    programs.clear();

    if (api::getRecorded(response) == api::REQUEST_FAILED) {
        return false;
    }

    std::copy(response["recorded"].begin(), response["recorded"].end(), std::back_inserter(programs));

    XBMC->Log(ADDON::LOG_NOTICE, "Updated recorded program: ammount = %d", programs.size());
    return true;
}

bool Recorded::remove(const std::string id)
{
    const auto success = api::deleteRecordedProgram(id) != api::REQUEST_FAILED;
    XBMC->Log(success ? ADDON::LOG_NOTICE : ADDON::LOG_ERROR,
        "Recording deletion: #%s", id.c_str());
    return success;
}

} // namespace epgstation
