/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_RULES_H_
#define SRC_EPGSTATION_RULES_H_
#include "epgstation/types.h"
#include <iostream>
#include <string>
#include <vector>

namespace epgstation {

class Rule {
public:
    std::vector<rule> rules;
    bool refresh();
    bool add(bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory);
    bool edit(int id, bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory);
    bool enable(int id, bool enabled);
};
} // namespace epgstation

#endif // SRC_EPGSTATION_RULES_H_
