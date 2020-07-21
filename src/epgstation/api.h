/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_API_H
#define EPGSTATION_API_H

#include "json/json.hpp"
#include <iostream>

namespace epgstation {
namespace api {
    extern const int REQUEST_FAILED;
    extern std::string baseURL;

    // GET /api/schedule
    int getSchedule(std::string type, nlohmann::json& response);

    // GET /api/recorded
    int getRecorded(nlohmann::json& response);

    // GET /api/reserves
    int getReserves(nlohmann::json& response);

    // DELETE /api/recorded/:id
    int deleteRecordedProgram(std::string id);

    // DELETE /api/reserves/:id
    int deleteReserves(std::string id);

    // DELETE /api/reserves/:id/skip
    int deleteReservesSkip(std::string id);

    // POST /api/reserves
    int postReserves(std::string id);

    // GET /api/rules
    int getRules(nlohmann::json& response);

    // POST /api/rules
    int postRules(bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory);

    // PUT /api/rules/:id
    int putRule(int id, bool enabled, std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, std::string directory);

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state);

    // PUT /api/scheduler
    int putScheduleUpdate();

    // GET /api/storage
    int getStorage(nlohmann::json& response);

} // namespace api
} // namespace epgstation

#endif /* end of include guard */
