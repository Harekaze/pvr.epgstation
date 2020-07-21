/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_STORAGE_H
#define EPGSTATION_STORAGE_H
#include <iostream>

namespace epgstation {
class Docs {
private:
    static std::string backendName;
    static std::string backendVersion;
    static std::string backendHostname;
    static bool fetchSystemInfo();

public:
    static const char* getBackendName();
    static const char* getBackendVersion();
    static const char* getBackendHostname();
};
} // namespace epgstation

#endif /* end of include guard */
