/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef SRC_EPGSTATION_API_H_
#define SRC_EPGSTATION_API_H_

#include "json/json.hpp"
#include <iostream>
#include <string>

namespace epgstation {
namespace api {
    extern const int REQUEST_FAILED;
    extern std::string baseURL;

    // GET /api/channels
    int getChannels(nlohmann::json& response);

    // GET /api/schedule?type=:type
    int getScheduleAll(const std::string type, nlohmann::json& response);

    // GET /api/schedule/:id?time=:time&days=:days
    int getSchedule(const std::string id, const char* time, const uint16_t days, nlohmann::json& response);

    // GET /api/recorded
    int getRecorded(nlohmann::json& response);

    // GET /api/reserves
    int getReserves(nlohmann::json& response);

    // GET /api/reserves/skips
    int getReservesSkips(nlohmann::json& response);

    // GET /api/reserves/conflicts
    int getReservesConflicts(nlohmann::json& response);

    // DELETE /api/recorded/:id
    int deleteRecordedProgram(const std::string id);

    // DELETE /api/reserves/:id
    int deleteReserves(const std::string id);

    // DELETE /api/reserves/:id/skip
    int deleteReservesSkip(const std::string id);

    // POST /api/reserves
    int postReserves(const std::string id);

    // GET /api/rules
    int getRules(nlohmann::json& response);

    // POST /api/rules
    int postRules(bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory);

    // PUT /api/rules/:id
    int putRule(int id, bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory);

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state);

    // PUT /api/scheduler
    int putScheduleUpdate();

    // GET /api/storage
    int getStorage(nlohmann::json& response);

    // GET /api/docs
    int getDocs(nlohmann::json& response);

} // namespace api
} // namespace epgstation

#endif // SRC_EPGSTATION_API_H_
