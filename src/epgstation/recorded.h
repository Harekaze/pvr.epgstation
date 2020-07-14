/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_RECORDED_H
#define EPGSTATION_RECORDED_H
#include <iostream>

#include "epgstation/genre.h"
#include "kodi/xbmc_pvr_types.h"
#include "picojson/picojson.h"

namespace epgstation {
class Recorded {
public:
    std::string recordedStreamingPath;
    std::string recordedThumbnailPath;
    std::vector<PVR_RECORDING> programs;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
