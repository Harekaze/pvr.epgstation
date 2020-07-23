/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "api.h"
#include "../base64/base64.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "json/json.hpp"

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

namespace epgstation {
namespace api {
    constexpr int REQUEST_FAILED = -1;
    std::string baseURL = "";

    int request(const std::string method, const std::string path, nlohmann::json* response = NULL, nlohmann::json body = NULL)
    {
        const auto url = baseURL + path;
        XBMC->Log(ADDON::LOG_DEBUG, "Request URL: %s", url.c_str());
        XBMC->Log(ADDON::LOG_DEBUG, "Request Method: %s", method.c_str());

        std::string text;
        void* handle = XBMC->CURLCreate(url.c_str());
        if (handle == NULL) {
            XBMC->Log(ADDON::LOG_ERROR, "Failed to create URL: %s", url.c_str());
            return REQUEST_FAILED;
        }

        if (method != "GET") {
            if (!XBMC->CURLAddOption(handle, XFILE::CURLOPTIONTYPE::CURL_OPTION_PROTOCOL, "customrequest", method.c_str())) {
                XBMC->Log(ADDON::LOG_ERROR, "Failed to set %s method to %s", method.c_str(), url.c_str());
                XBMC->CloseFile(handle);
                return REQUEST_FAILED;
            }
        }

        if (body != NULL) {
            XBMC->Log(ADDON::LOG_DEBUG, "Setting request header");
            if (!XBMC->CURLAddOption(handle, XFILE::CURLOPTIONTYPE::CURL_OPTION_HEADER, "Content-Type", "application/json")) {
                XBMC->Log(ADDON::LOG_ERROR, "Failed to set content-type to %s", url.c_str());
                XBMC->CloseFile(handle);
                return REQUEST_FAILED;
            }

            XBMC->Log(ADDON::LOG_DEBUG, "Setting request body");
            const std::string data = base64_encode(body.dump());
            if (!XBMC->CURLAddOption(handle, XFILE::CURLOPTIONTYPE::CURL_OPTION_PROTOCOL, "postdata", data.c_str())) {
                XBMC->Log(ADDON::LOG_ERROR, "Failed to set post data to %s", url.c_str());
                XBMC->CloseFile(handle);
                return REQUEST_FAILED;
            }
        }

        XBMC->Log(ADDON::LOG_DEBUG, "Open url for requesting");
        if (!XBMC->CURLOpen(handle, XFILE::READ_NO_CACHE)) {
            XBMC->Log(ADDON::LOG_ERROR, "Failed to open URL: %s", url.c_str());
            XBMC->CloseFile(handle);
            return REQUEST_FAILED;
        }

        if (response != NULL) {
            XBMC->Log(ADDON::LOG_DEBUG, "Read response body");

            const unsigned int buffer_size = 4096;
            char buffer[buffer_size];
            while (int bytesRead = XBMC->ReadFile(handle, buffer, buffer_size)) {
                text.append(buffer, bytesRead);
            }
        }

        XBMC->CloseFile(handle);

        if (response != NULL && !text.empty()) {
            try {
                *response = nlohmann::json::parse(text);
            } catch (nlohmann::json::parse_error err) {
                XBMC->Log(ADDON::LOG_ERROR, "Failed to parse JSON string: %s", err.what());
                return REQUEST_FAILED;
            }
        }

        XBMC->Log(ADDON::LOG_DEBUG, "Complete API request for %s", url.c_str());

        return 0;
    }

    // GET /api/schedule?type=:type
    int getSchedule(const std::string type, nlohmann::json& response)
    {
        const auto apiPath = "schedule?type=" + type;
        return request("GET", apiPath, &response);
    }

    // GET /api/recorded
    int getRecorded(nlohmann::json& response)
    {
        constexpr char apiPath[] = "recorded";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves
    int getReserves(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves/skips
    int getReservesSkips(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves/skips";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves/conflicts
    int getReservesConflicts(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves/conflicts";
        return request("GET", apiPath, &response);
    }

    // DELETE /api/recorded/:id
    int deleteRecordedProgram(const std::string id)
    {
        const auto apiPath = "recorded/" + id;
        return request("DELETE", apiPath);
    }

    // DELETE /api/reserves/:id
    int deleteReserves(const std::string id)
    {
        const auto apiPath = "reserves/" + id;
        return request("DELETE", apiPath);
    }

    // DELETE /api/reserves/:id/skip
    int deleteReservesSkip(const std::string id)
    {
        const auto apiPath = "reserves/" + id + "/skip";
        return request("DELETE", apiPath);
    }

    // POST /api/reserves
    int postReserves(const std::string id)
    {
        nlohmann::json body = {
            { "programId", std::stoul(id) },
            { "allowEndLack", true },
        };
        constexpr char apiPath[] = "reserves";
        return request("POST", apiPath, NULL, body);
    }

    // GET /api/rules
    int getRules(nlohmann::json& response)
    {
        constexpr char apiPath[] = "rules";
        return request("GET", apiPath, &response);
    }

    nlohmann::json createRulePayload(bool enabled, const std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        unsigned int newWeekdays = weekdays ^ PVR_WEEKDAY_SUNDAY;
        newWeekdays <<= 1;
        if (weekdays & PVR_WEEKDAY_SUNDAY) {
            newWeekdays |= 0x01;
        }
        nlohmann::json body = {
            { "search", {
                            { "keyword", searchText },
                            { "title", true },
                            { "description", fullText },
                            { "week", newWeekdays },
                        } },
            { "option", {
                            { "enable", enabled },
                            { "allowEndLack", true },
                        } }
        };

        if (channelId == -1) {
            body["search"]["GR"] = true;
            body["search"]["BS"] = true;
            body["search"]["CS"] = true;
            body["search"]["SKY"] = true;
        } else {
            body["search"]["station"] = (unsigned int)channelId;
        }

        if (!anytime) {
            body["search"]["startTime"] = startHour;
            body["search"]["timeRange"] = (24 + endHour - startHour) % 24;
        }

        if (!directory.empty()) {
            body["option"]["directory"] = directory;
        }
        return body;
    }

    // POST /api/rules
    int postRules(bool enabled, const std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        constexpr char apiPath[] = "rules";
        nlohmann::json body = createRulePayload(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory);
        return request("POST", apiPath, NULL, body);
    }

    // PUT /api/rules/:id
    int putRule(int id, bool enabled, const std::string searchText, bool fullText, int channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        const auto apiPath = "rules/" + std::to_string(id);
        nlohmann::json body = createRulePayload(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory);
        return request("PUT", apiPath, NULL, body);
    }

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state)
    {
        const auto apiPath = "rules/" + std::to_string(id) + (state ? "/enable" : "/disable");
        return request("PUT", apiPath);
    }

    // PUT /api/schedule/update
    int putScheduleUpdate()
    {
        constexpr char apiPath[] = "schedule/update";
        return request("PUT", apiPath);
    }

    // GET /api/storage
    int getStorage(nlohmann::json& response)
    {
        constexpr char apiPath[] = "storage";
        return request("GET", apiPath, &response);
    }

    // GET /api/docs
    int getDocs(nlohmann::json& response)
    {
        constexpr char apiPath[] = "docs";
        return request("GET", apiPath, &response);
    }

} // namespace api
} // namespace epgstation
