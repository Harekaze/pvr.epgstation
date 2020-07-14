/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifndef EPGSTATION_API_H
#define EPGSTATION_API_H

#include <iostream>
#include "picojson/picojson.h"

namespace epgstation {
	namespace api {
		extern const int REQUEST_FAILED;
		extern std::string baseURL;

		// GET /schedule.json
		int getSchedule(picojson::value &response);

		// GET /recorded.json
		int getRecorded(picojson::value &response);

		// GET /reserves.json
		int getReserves(picojson::value &response);

		// DELETE /recorded/:id.json
		int deleteRecordedProgram(std::string id);

		// DELETE /recording/:id.json
		int deleteRecordingProgram(std::string id);

		// PUT /reserves/:id/skip.json
		int putReservesSkip(std::string id);

		// PUT /reserves/:id/unskip.json
		int putReservesUnskip(std::string id);

		// PUT /program/:id.json
		int putProgram(std::string id);

		// GET /rules.json
		int getRules(picojson::value &response);

		// POST /rules.json
		int postRule(std::string type, std::string channel, std::string title, std::string genre);

		// PUT /rules/:id/:action.json
		int putRuleAction(int id, bool state);

		// DELETE /reserves/:id.json
		int deleteReserves(std::string id);

		// PUT /scheduler.json
		int putScheduler();

		// GET /storage.json
		int getStorage(picojson::value &response);

	} // namespace api
} // namespace epgstation

#endif /* end of include guard */
