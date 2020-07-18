/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_RULES_H
#define EPGSTATION_RULES_H
#include "epgstation/types.h"
#include <iostream>

namespace epgstation {

class Rule {
public:
    std::vector<rule> rules;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
