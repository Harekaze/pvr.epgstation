/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/api.h"
#include "../base64/base64.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "json/json.hpp"
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

namespace epgstation {
namespace api {
    const int REQUEST_FAILED = -1;
    std::string baseURL = "";

    int request(const std::string method, const std::string path, nlohmann::json* response = nullptr, nlohmann::json body = nullptr)
    {
        const auto url = baseURL + path;
        XBMC->Log(ADDON::LOG_DEBUG, "Request URL: %s", url.c_str());
        XBMC->Log(ADDON::LOG_DEBUG, "Request Method: %s", method.c_str());

        std::string text;
        void* handle = XBMC->CURLCreate(url.c_str());
        if (handle == nullptr) {
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

        if (body != nullptr) {
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
        constexpr uint8_t retry_limit = 3;
        uint8_t retry = 0;
        do {
            if (XBMC->CURLOpen(handle, XFILE::READ_NO_CACHE)) {
                break;
            }
            XBMC->Log(ADDON::LOG_ERROR, "Failed to open URL: %s (retry %d)", url.c_str(), retry);
            if (++retry >= retry_limit) {
                XBMC->CloseFile(handle);
                return REQUEST_FAILED;
            }
            sleep(3);
        } while (1);

        if (response != nullptr) {
            XBMC->Log(ADDON::LOG_DEBUG, "Read response body");

            constexpr uint16_t buffer_size = 4096;
            auto buffer = new char[buffer_size];
            while (auto bytesRead = XBMC->ReadFile(handle, buffer, buffer_size)) {
                text.append(buffer, bytesRead);
            }
            delete[] buffer;
        }

        XBMC->CloseFile(handle);

        if (response != nullptr && !text.empty()) {
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

    // GET /api/channels
    int getChannels(nlohmann::json& response)
    {
        const auto apiPath = "channels";
        return request("GET", apiPath, &response);
    }

    // GET /api/schedule?type=:type
    int getScheduleAll(const std::string type, nlohmann::json& response)
    {
        const auto apiPath = "schedule?type=" + type;
        return request("GET", apiPath, &response);
    }

    // GET /api/schedule/:id?time=:time&days=:days
    int getSchedule(const std::string id, const char* time, const uint16_t days, nlohmann::json& response)
    {
        const auto apiPath = "schedule/" + id + "?time=" + time + "&days=" + std::to_string(days);
        return request("GET", apiPath, &response);
    }

    // GET /api/recorded
    int getRecorded(nlohmann::json& response)
    {
        constexpr char apiPath[] = "recorded?limit=65535";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves
    int getReserves(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves?limit=65535";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves/skips
    int getReservesSkips(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves/skips?limit=65535";
        return request("GET", apiPath, &response);
    }

    // GET /api/reserves/conflicts
    int getReservesConflicts(nlohmann::json& response)
    {
        constexpr char apiPath[] = "reserves/conflicts?limit=65535";
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
            { "programId", std::stoull(id) },
            { "allowEndLack", true },
        };
        constexpr char apiPath[] = "reserves";
        return request("POST", apiPath, nullptr, body);
    }

    // GET /api/rules
    int getRules(nlohmann::json& response)
    {
        constexpr char apiPath[] = "rules?limit=65535";
        return request("GET", apiPath, &response);
    }

    nlohmann::json createRulePayload(bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        uint16_t newWeekdays = weekdays ^ PVR_WEEKDAY_SUNDAY;
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

        if (channelId == 0) {
            body["search"]["GR"] = true;
            body["search"]["BS"] = true;
            body["search"]["CS"] = true;
            body["search"]["SKY"] = true;
        } else {
            body["search"]["station"] = channelId;
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
    int postRules(bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        constexpr char apiPath[] = "rules";
        nlohmann::json body = createRulePayload(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory);
        return request("POST", apiPath, nullptr, body);
    }

    // PUT /api/rules/:id
    int putRule(int id, bool enabled, const std::string searchText, bool fullText, uint64_t channelId, unsigned int weekdays, unsigned int startHour, unsigned int endHour, bool anytime, const std::string directory)
    {
        const auto apiPath = "rules/" + std::to_string(id);
        nlohmann::json body = createRulePayload(enabled, searchText, fullText, channelId, weekdays, startHour, endHour, anytime, directory);
        return request("PUT", apiPath, nullptr, body);
    }

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state)
    {
        const auto apiPath = "rules/" + std::to_string(id) + (state ? "/enable" : "/disable");
        return request("PUT", apiPath);
    }

    // DELETE /api/rules/:id
    int deleteRule(int id)
    {
        const auto apiPath = "rules/" + std::to_string(id);
        return request("DELETE", apiPath);
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
