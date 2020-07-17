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

class recorded {
public:
    unsigned int id;
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

    friend void from_json(const nlohmann::json& j, recorded& r)
    {
        j.at("id").get_to(r.id);
        j.at("programId").get_to(r.programId);
        j.at("channelId").get_to(r.channelId);
        j.at("channelType").get_to(r.channelType);
        j.at("name").get_to(r.name);
        if (j.contains("description")) {
            j.at("description").get_to(r.description);
        }
        if (j.contains("extended")) {
            j.at("extended").get_to(r.extended);
        }
        r.startAt = j["startAt"].get<unsigned long>() / 1000;
        r.endAt = j["endAt"].get<unsigned long>() / 1000;
        j.at("genre1").get_to(r.genre1);
        j.at("genre2").get_to(r.genre2);
        j.at("recording").get_to(r.recording);
        j.at("hasThumbnail").get_to(r.hasThumbnail);
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
