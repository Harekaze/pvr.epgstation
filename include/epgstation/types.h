/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_TYPES_H
#define EPGSTATION_TYPES_H
#include "json/json.hpp"
#include <string>

#define OPTIONAL_JSON_FROM(v)  \
    if (j.contains(#v)) {      \
        NLOHMANN_JSON_FROM(v); \
    }

namespace epgstation {
class storage {
public:
    long long total;
    long long used;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(storage, total, used);
};

enum ReservedState {
    STATE_RESERVED,
    STATE_CONFLICT,
    STATE_SKIPPED,
};

class program {
public:
    unsigned long id;
    unsigned long programId;
    unsigned long channelId;
    unsigned long eventId;
    std::string channelType;
    std::string name;
    std::string description;
    std::string extended;
    time_t startAt;
    time_t endAt;
    unsigned int genre1 = 0xff;
    unsigned int genre2 = 0xff;
    bool recording;
    bool hasThumbnail;
    int ruleId; // Optional for reserved program
    ReservedState state; // Optional for reserved program

    friend void from_json(const nlohmann::json& j, program& t)
    {
        NLOHMANN_JSON_FROM(id);
        OPTIONAL_JSON_FROM(programId);
        OPTIONAL_JSON_FROM(eventId);
        NLOHMANN_JSON_FROM(channelId);
        NLOHMANN_JSON_FROM(channelType);
        NLOHMANN_JSON_FROM(name);
        OPTIONAL_JSON_FROM(description);
        OPTIONAL_JSON_FROM(extended);
        t.startAt = (time_t)(j["startAt"].get<unsigned long>() / 1000);
        t.endAt = (time_t)(j["endAt"].get<unsigned long>() / 1000);
        OPTIONAL_JSON_FROM(genre1);
        OPTIONAL_JSON_FROM(genre2);
        OPTIONAL_JSON_FROM(recording);
        OPTIONAL_JSON_FROM(hasThumbnail);
    }
};

class channel {
public:
    unsigned long id;
    unsigned int serviceId;
    unsigned int networkId;
    std::string name;
    bool hasLogoData;
    std::string channelType;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(channel, id, serviceId, networkId, name, hasLogoData, channelType);
};

class rule {
public:
    unsigned int id;
    std::string keyword;
    bool title = true;
    bool description = false;
    bool enable = true;
    unsigned int week = 0;
    int station = -1;
    unsigned int startTime = 0;
    unsigned int timeRange = 0;
    std::string directory = "";

    friend void from_json(const nlohmann::json& j, rule& t)
    {
        NLOHMANN_JSON_FROM(id);
        NLOHMANN_JSON_FROM(keyword);
        OPTIONAL_JSON_FROM(title);
        OPTIONAL_JSON_FROM(description);
        NLOHMANN_JSON_FROM(enable);
        NLOHMANN_JSON_FROM(week);
        t.week = ((t.week & 0b00000001) << 6) | ((t.week & 0b01111110) >> 1);
        OPTIONAL_JSON_FROM(station);
        OPTIONAL_JSON_FROM(startTime);
        OPTIONAL_JSON_FROM(timeRange);
        OPTIONAL_JSON_FROM(directory);
    }
};

} // namespace epgstation

#endif /* end of include guard */
