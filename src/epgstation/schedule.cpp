/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "schedule.h"
#include "api.h"
#include "kodi/libXBMC_addon.h"

extern ADDON::CHelper_libXBMC_addon *XBMC;

namespace epgstation {
	bool Schedule::refresh() {
		picojson::value response;
		const time_t refreshInterval = 10*60; // every 10 minutes
		static time_t lastUpdated;
		time_t now;

		time(&now);
		if (now - lastUpdated < refreshInterval) {
			return true;
		}

		if (epgstation::api::getSchedule(response) == epgstation::api::REQUEST_FAILED) {
			return false;
		}

		schedule.clear();
		channelGroups.clear();

		for (picojson::value &a: response.get<picojson::array>()) {
			picojson::object &o = a.get<picojson::object>();
			if (o["programs"].get<picojson::array>().empty()) {
				continue;
			}

			PVR_CHANNEL ch;
			ch.iUniqueId = o["sid"].is<double>() ? (int)(o["sid"].get<double>()) : 0;
			ch.bIsRadio = false;
			ch.bIsHidden = false;
			ch.iChannelNumber = o["n"].is<double>() ? (int)((o["n"].get<double>())) + 1 : 0;
			ch.iSubChannelNumber = o["nid"].is<double>() ? (int)(o["nid"].get<double>()) : 0;
			// use channel id as name instead when name field isn't available.
			strncpy(ch.strChannelName, o["name"].is<std::string>() ? o["name"].get<std::string>().c_str() : o["id"].get<std::string>().c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);

			if (o["hasLogoData"].get<bool>()) {
				snprintf(ch.strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1,
					(const char*)(epgstation::api::baseURL + channelLogoPath).c_str(),
					o["id"].get<std::string>().c_str());
			} else {
				ch.strIconPath[0] = '\0';
			}

			const std::string strChannelType = o["type"].get<std::string>();
			channelGroups[strChannelType].push_back(ch);

			for (picojson::value &ps: o["programs"].get<picojson::array>()) {
				picojson::object &p = ps.get<picojson::object>();
				struct EPG_PROGRAM epg;
				char *endptr;

				epg.startTime = (time_t)(p["start"].get<double>() / 1000);
				epg.endTime = (time_t)(p["end"].get<double>() / 1000);
				epg.strUniqueBroadcastId = p["id"].get<std::string>();
				const std::string strSubstrId = epg.strUniqueBroadcastId.substr(epg.strUniqueBroadcastId.size() - 6, 6);
				epg.iUniqueBroadcastId = strtoul(strSubstrId.c_str(), &endptr, 36);
				epg.strTitle = p["title"].get<std::string>();
				epg.strEpisodeName = p["subTitle"].is<std::string>() ? p["subTitle"].get<std::string>() : "";
				epg.strPlotOutline = p["description"].is<std::string>() ? p["description"].get<std::string>() : p["subTitle"].get<std::string>();
				epg.strPlot = p["detail"].is<std::string>() ? p["detail"].get<std::string>() : "";
				epg.strOriginalTitle = p["fullTitle"].is<std::string>() ? p["fullTitle"].get<std::string>() : "";
				epg.strGenreDescription = p["category"].is<std::string>() ? p["category"].get<std::string>() : "";
				epg.iEpisodeNumber = p["episode"].is<double>() ? (unsigned int)(p["episode"].get<double>()) : 0;

				schedule[ch.iUniqueId].push_back(epg);
			}
		}

		XBMC->Log(ADDON::LOG_NOTICE, "Updated schedule: channel ammount = %d", schedule.size());

		lastUpdated = now;
		return true;
	}
}
