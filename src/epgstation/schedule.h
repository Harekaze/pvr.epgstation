/*
 *         Copyright (C) 2015-2018 Yuki MIZUNO
 *         https://github.com/Harekaze/pvr.epgstation/
 *
 *
 * This file is part of pvr.epgstation.
 *
 * pvr.epgstation is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pvr.epgstation is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pvr.epgstation.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef EPGSTATION_SCHEDULE_H
#define EPGSTATION_SCHEDULE_H
#include <iostream>
#include <map>

#include "picojson/picojson.h"
#include "epgstation/genre.h"
#include "kodi/xbmc_pvr_types.h"
#include "kodi/xbmc_epg_types.h"

namespace epgstation {
	struct EPG_PROGRAM {
		unsigned int iUniqueBroadcastId;
		unsigned int iEpisodeNumber;
		time_t startTime;
		time_t endTime;
		std::string strUniqueBroadcastId;
		std::string strTitle;
		std::string strPlotOutline;
		std::string strPlot;
		std::string strOriginalTitle;
		std::string strEpisodeName;
		std::string strGenreDescription;
	};
	class Schedule {
		public:
			std::string channelLogoPath;
			std::string liveStreamingPath;
			std::map<unsigned int, std::vector<EPG_PROGRAM>> schedule;
			std::map<std::string, std::vector<PVR_CHANNEL>> channelGroups;
			bool refresh();
	};
} // namespace epgstation

#endif /* end of include guard */
