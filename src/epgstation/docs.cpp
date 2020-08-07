/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/docs.h"
#include "epgstation/api.h"
#include "kodi/libXBMC_addon.h"
#include "json/json.hpp"
#include <string>

namespace epgstation {
bool Docs::fetchSystemInfo()
{
    nlohmann::json response;
    if (api::getDocs(response) == api::REQUEST_FAILED) {
        return false;
    }

    std::string name = response["info"]["title"].get<std::string>();
    std::string version = response["info"]["version"].get<std::string>();
    std::string hostname = response["host"].get<std::string>();

    snprintf(Docs::backendName, sizeof(Docs::backendName), "%s", name.c_str());
    snprintf(Docs::backendVersion, sizeof(Docs::backendVersion), "%s", version.c_str());
    snprintf(Docs::backendHostname, sizeof(Docs::backendHostname), "%s", hostname.c_str());

    return true;
}

const char* Docs::getBackendName()
{
    if (std::char_traits<char>::length(Docs::backendName) == 0) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendName;
}

const char* Docs::getBackendVersion()
{
    if (std::char_traits<char>::length(Docs::backendVersion) == 0) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendVersion;
}

const char* Docs::getBackendHostname()
{
    if (std::char_traits<char>::length(Docs::backendHostname) == 0) {
        Docs::fetchSystemInfo();
    }
    return Docs::backendHostname;
}

char Docs::backendName[128] = { 0 };
char Docs::backendVersion[128] = { 0 };
char Docs::backendHostname[128] = { 0 };

} // namespace epgstation
