/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_API_H
#define EPGSTATION_API_H

#include "picojson/picojson.h"
#include <iostream>

namespace epgstation {
namespace api {
    extern const int REQUEST_FAILED;
    extern std::string baseURL;

    // GET /api/schedule
    int getSchedule(picojson::value& response);

    // GET /api/recorded
    int getRecorded(picojson::value& response);

    // GET /api/reserves
    int getReserves(picojson::value& response);

    // DELETE /api/recorded/:id
    int deleteRecordedProgram(std::string id);

    // DELETE /api/reserves/:id
    int deleteReserves(std::string id);

    // DELETE /api/reserves/:id/skip
    int deleteReservesSkip(std::string id);

    // POST /api/reserves
    int postReserves(std::string id);

    // GET /api/rules
    int getRules(picojson::value& response);

    // POST /api/rules
    int postRules(std::string type,
        std::string channel,
        std::string title,
        std::string genre);

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state);

    // PUT /api/scheduler
    int putScheduleUpdate();

    // GET /api/storage
    int getStorage(picojson::value& response);

} // namespace api
} // namespace epgstation

#endif /* end of include guard */
