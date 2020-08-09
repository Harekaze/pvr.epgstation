/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_CHANNELS_H_
#define SRC_EPGSTATION_CHANNELS_H_
#include "epgstation/types.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace epgstation {
class Channels {
public:
    std::string channelLogoPath;
    std::string liveStreamingPath;
    std::vector<channel> channels;
    bool refresh();
};
} // namespace epgstation

#endif // SRC_EPGSTATION_CHANNELS_H_
