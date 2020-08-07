/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_STORAGE_H_
#define SRC_EPGSTATION_STORAGE_H_
#include <iostream>

#include "kodi/xbmc_pvr_types.h"

namespace epgstation {
class Storage {
public:
    static PVR_ERROR getStorageInfo(long long* used, long long* total);
};
} // namespace epgstation

#endif // SRC_EPGSTATION_STORAGE_H_
