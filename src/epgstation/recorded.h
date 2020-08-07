/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_RECORDED_H_
#define SRC_EPGSTATION_RECORDED_H_
#include "epgstation/types.h"
#include <iostream>
#include <string>
#include <vector>

namespace epgstation {
class Recorded {
public:
    std::string recordedStreamingPath;
    std::string recordedThumbnailPath;
    std::vector<program> programs;
    bool refresh();
    bool remove(const std::string id);
};
} // namespace epgstation

#endif // SRC_EPGSTATION_RECORDED_H_
