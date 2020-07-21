/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "docs.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"

namespace epgstation {
bool Docs::fetchSystemInfo()
{
    nlohmann::json response;
    if (api::getDocs(response) == api::REQUEST_FAILED) {
        return false;
    }

    Docs::backendName = response["info"]["title"].get<std::string>();
    Docs::backendVersion = response["info"]["version"].get<std::string>();
    Docs::backendHostname = response["host"].get<std::string>();

    return true;
}

const char* Docs::getBackendName()
{
    if (Docs::backendName.empty()) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendName.c_str();
}

const char* Docs::getBackendVersion()
{
    if (Docs::backendVersion.empty()) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendVersion.c_str();
}

const char* Docs::getBackendHostname()
{
    if (Docs::backendHostname.empty()) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendHostname.c_str();
}

std::string Docs::backendName = "";
std::string Docs::backendVersion = "";
std::string Docs::backendHostname = "";

}
