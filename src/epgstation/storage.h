/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_STORAGE_H
#define EPGSTATION_STORAGE_H
#include <iostream>

#include "kodi/xbmc_pvr_types.h"

namespace epgstation {
class Storage {
public:
    static PVR_ERROR getStorageInfo(long long* used, long long* total);
};
} // namespace epgstation

#endif /* end of include guard */
