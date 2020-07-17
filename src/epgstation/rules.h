/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_RULES_H
#define EPGSTATION_RULES_H
#include <iostream>

#include "epgstation/genre.h"
#include "kodi/xbmc_pvr_types.h"

namespace epgstation {
struct RULE_ITEM {
    unsigned int iIndex;
    std::string strTitle;
    std::string strEpgSearchString;
    unsigned int iClientChannelUid;
    PVR_TIMER_STATE state;
    bool bFullTextEpgSearch;
    bool bIsDisabled;
    int iGenreType;
    int iGenreSubType;
};
class Rule {
public:
    std::vector<RULE_ITEM> rules;
    bool refresh();
};
} // namespace epgstation

#endif /* end of include guard */
