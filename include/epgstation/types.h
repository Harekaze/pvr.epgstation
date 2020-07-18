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

class program {
public:
    unsigned long id;
    unsigned long programId;
    unsigned long channelId;
    std::string channelType;
    std::string name;
    std::string description;
    std::string extended;
    time_t startAt;
    time_t endAt;
    unsigned int genre1;
    unsigned int genre2;
    bool recording;
    bool hasThumbnail;
    int ruleId; // Optional for reserved program

    friend void from_json(const nlohmann::json& j, program& t)
    {
        NLOHMANN_JSON_FROM(id);
        OPTIONAL_JSON_FROM(programId);
        NLOHMANN_JSON_FROM(channelId);
        NLOHMANN_JSON_FROM(channelType);
        NLOHMANN_JSON_FROM(name);
        OPTIONAL_JSON_FROM(description);
        OPTIONAL_JSON_FROM(extended);
        t.startAt = j["startAt"].get<unsigned long>() / 1000;
        t.endAt = j["endAt"].get<unsigned long>() / 1000;
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
    unsigned int remoteControlKeyId;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(channel, id, serviceId, networkId, name, hasLogoData, channelType, remoteControlKeyId);
};
} // namespace epgstation

#endif /* end of include guard */
