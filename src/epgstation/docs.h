/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_DOCS_H_
#define SRC_EPGSTATION_DOCS_H_
#include <iostream>

namespace epgstation {
class Docs {
private:
    static char backendName[128];
    static char backendVersion[128];
    static char backendHostname[128];
    static bool fetchSystemInfo();

public:
    static const char* getBackendName();
    static const char* getBackendVersion();
    static const char* getBackendHostname();
};
} // namespace epgstation

#endif // SRC_EPGSTATION_DOCS_H_
