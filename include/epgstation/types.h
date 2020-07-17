/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_TYPES_H
#define EPGSTATION_TYPES_H
#include "json/json.hpp"
#include <string>

namespace epgstation {
class storage {
public:
    long long total;
    long long used;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(storage, total, used);
};
} // namespace epgstation

#endif /* end of include guard */
