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
    uint32_t id;
    uint32_t programId;
    uint32_t channelId;
    uint32_t eventId;
    std::string channelType;
    std::string name;
    std::string description;
    std::string extended;
    time_t startAt;
    time_t endAt;
    uint8_t genre1 = 0xff;
    uint8_t genre2 = 0xff;
    bool recording;
    bool hasThumbnail;
    bool original;
    std::vector<std::pair<uint8_t, std::string>> encoded;
    int16_t ruleId; // Optional for reserved program
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
        t.startAt = static_cast<time_t>(j["startAt"].get<long long>() / 1000ll);
        t.endAt = static_cast<time_t>(j["endAt"].get<long long>() / 1000ll);
        OPTIONAL_JSON_FROM(genre1);
        OPTIONAL_JSON_FROM(genre2);
        OPTIONAL_JSON_FROM(recording);
        OPTIONAL_JSON_FROM(hasThumbnail);
        OPTIONAL_JSON_FROM(original);
        if (j.contains("encoded") && j["encoded"].is_array()) {
            for (auto e : j["encoded"]) {
                t.encoded.push_back(std::make_pair(e["encodedId"], e["name"]));
            }
        }
    }
};

class channel {
public:
    uint32_t id;
    uint16_t serviceId;
    uint16_t networkId;
    std::string name;
    bool hasLogoData;
    std::string channelType;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(channel, id, serviceId, networkId, name, hasLogoData, channelType);
};

class rule {
public:
    uint16_t id;
    std::string keyword;
    bool title = true;
    bool description = false;
    bool enable = true;
    uint16_t week = 0;
    int16_t station = -1;
    uint16_t startTime = 0;
    uint16_t timeRange = 0;
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
