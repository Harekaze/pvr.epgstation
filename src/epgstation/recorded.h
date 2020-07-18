/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_RECORDED_H
#define EPGSTATION_RECORDED_H
#include "epgstation/types.h"
#include <iostream>

namespace epgstation {
class Recorded {
public:
    std::string recordedStreamingPath;
    std::string recordedThumbnailPath;
    std::vector<program> programs;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
