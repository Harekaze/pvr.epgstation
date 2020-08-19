/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_TYPES_H
#define EPGSTATION_TYPES_H
#include "json/json.hpp"
#include <string>

#define OPTIONAL_JSON_FROM(v)                                             \
    if (nlohmann_json_j.contains(#v) && !nlohmann_json_j[#v].is_null()) { \
        NLOHMANN_JSON_FROM(v);                                            \
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
    uint64_t id;
    uint64_t programId;
    uint64_t channelId;
    uint64_t eventId;
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
    int16_t ruleId = 0; // Optional for reserved program
    ReservedState state; // Optional for reserved program

    friend void from_json(const nlohmann::json& nlohmann_json_j, program& nlohmann_json_t)
    {
        NLOHMANN_JSON_FROM(id);
        OPTIONAL_JSON_FROM(programId);
        OPTIONAL_JSON_FROM(eventId);
        NLOHMANN_JSON_FROM(channelId);
        NLOHMANN_JSON_FROM(channelType);
        NLOHMANN_JSON_FROM(name);
        OPTIONAL_JSON_FROM(description);
        OPTIONAL_JSON_FROM(extended);
        nlohmann_json_t.startAt = static_cast<time_t>(nlohmann_json_j["startAt"].get<long long>() / 1000ll);
        nlohmann_json_t.endAt = static_cast<time_t>(nlohmann_json_j["endAt"].get<long long>() / 1000ll);
        OPTIONAL_JSON_FROM(genre1);
        OPTIONAL_JSON_FROM(genre2);
        OPTIONAL_JSON_FROM(recording);
        OPTIONAL_JSON_FROM(hasThumbnail);
        OPTIONAL_JSON_FROM(original);
        if (nlohmann_json_j.contains("encoded") && nlohmann_json_j["encoded"].is_array()) {
            std::transform(nlohmann_json_j["encoded"].begin(), nlohmann_json_j["encoded"].end(), std::back_inserter(nlohmann_json_t.encoded), [](const nlohmann::json& e) {
                return std::make_pair(e["encodedId"], e["name"]);
            });
        }
    }
};

class channel {
public:
    uint64_t id;
    uint32_t serviceId;
    uint32_t networkId;
    std::string name;
    bool hasLogoData;
    std::string channelType;
    uint8_t channelTypeId;
    uint8_t type;
    uint8_t remoteControlKeyId;
    std::string channel;
    int internalId; // for internal use

    friend void from_json(const nlohmann::json& nlohmann_json_j, epgstation::channel& nlohmann_json_t)
    {
        NLOHMANN_JSON_FROM(id);
        NLOHMANN_JSON_FROM(serviceId);
        NLOHMANN_JSON_FROM(networkId);
        NLOHMANN_JSON_FROM(name);
        NLOHMANN_JSON_FROM(hasLogoData);
        NLOHMANN_JSON_FROM(channelType);
        OPTIONAL_JSON_FROM(channelTypeId);
        NLOHMANN_JSON_FROM(type);
        OPTIONAL_JSON_FROM(remoteControlKeyId);
        OPTIONAL_JSON_FROM(channel);
    }
};

class rule {
public:
    uint32_t id;
    std::string keyword;
    bool title = true;
    bool description = false;
    bool enable = true;
    uint16_t week = 0;
    uint64_t station = 0;
    uint16_t startTime = 0;
    uint16_t timeRange = 0;
    std::string directory = "";

    friend void from_json(const nlohmann::json& nlohmann_json_j, rule& nlohmann_json_t)
    {
        NLOHMANN_JSON_FROM(id);
        NLOHMANN_JSON_FROM(keyword);
        OPTIONAL_JSON_FROM(title);
        OPTIONAL_JSON_FROM(description);
        NLOHMANN_JSON_FROM(enable);
        NLOHMANN_JSON_FROM(week);
        nlohmann_json_t.week = ((nlohmann_json_t.week & 0b00000001) << 6) | ((nlohmann_json_t.week & 0b01111110) >> 1);
        OPTIONAL_JSON_FROM(station);
        OPTIONAL_JSON_FROM(startTime);
        OPTIONAL_JSON_FROM(timeRange);
        OPTIONAL_JSON_FROM(directory);
    }
};

} // namespace epgstation

#endif /* end of include guard */
