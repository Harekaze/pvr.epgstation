/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "api.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

namespace epgstation {
namespace api {
    const int REQUEST_FAILED = -1;
    std::string baseURL = "";

    int requestGET(std::string apiPath, picojson::value& response)
    {
        std::string text;
        const std::string url = baseURL + apiPath;
        if (void* handle = XBMC->OpenFile(url.c_str(), 0)) {
            const unsigned int buffer_size = 4096;
            char buffer[buffer_size];
            text.clear();
            while (int bytesRead = XBMC->ReadFile(handle, buffer, buffer_size)) {
                text.append(buffer, bytesRead);
            }
            XBMC->CloseFile(handle);
        } else {
            XBMC->Log(ADDON::LOG_ERROR, "[%s] Request failed", apiPath.c_str());
            XBMC->QueueNotification(ADDON::QUEUE_ERROR, "[%s] Request failed", apiPath.c_str());
            return REQUEST_FAILED;
        }

        const std::string err = picojson::parse(response, text);
        if (!err.empty()) {
            XBMC->Log(ADDON::LOG_ERROR, "[%s] Failed to parse JSON string: %s", apiPath.c_str(), err.c_str());
            XBMC->QueueNotification(ADDON::QUEUE_ERROR, "[%s] Failed to parse JSON string: %s", apiPath.c_str(), err.c_str());
            return REQUEST_FAILED;
        }

        return text.length();
    }

    int requestDELETE(std::string apiPath)
    {
        const std::string url = baseURL + apiPath;
        if (void* handle = XBMC->OpenFileForWrite(url.c_str(), 0)) {
            const unsigned int buffer_size = 20;
            const char buffer[] = "{\"_method\":\"DELETE\"}";
            XBMC->WriteFile(handle, buffer, buffer_size);
            XBMC->CloseFile(handle);
            return 0;
        } else {
            return REQUEST_FAILED;
        }
    }

    int requestPUT(std::string apiPath)
    {
        const std::string url = baseURL + apiPath;
        if (void* handle = XBMC->OpenFileForWrite(url.c_str(), 0)) {
            const unsigned int buffer_size = 17;
            const char buffer[] = "{\"_method\":\"PUT\"}";
            XBMC->WriteFile(handle, buffer, buffer_size);
            XBMC->CloseFile(handle);
            return 0;
        } else {
            return REQUEST_FAILED;
        }
    }

    int requestPOST(std::string apiPath, const char buffer[], const unsigned int buffer_size)
    {
        const std::string url = baseURL + apiPath;
        if (void* handle = XBMC->OpenFileForWrite(url.c_str(), 0)) {
            XBMC->WriteFile(handle, buffer, buffer_size);
            XBMC->CloseFile(handle);
            return 0;
        } else {
            return REQUEST_FAILED;
        }
    }

    // FIXME: Support other types
    // GET /api/schedule?type=GR
    int getSchedule(picojson::value& response)
    {
        const std::string apiPath = "schedule?type=GR";
        return requestGET(apiPath, response);
    }

    // GET /api/recorded
    int getRecorded(picojson::value& response)
    {
        const std::string apiPath = "recorded";
        return requestGET(apiPath, response);
    }

    // GET /api/reserves
    int getReserves(picojson::value& response)
    {
        const std::string apiPath = "reserves";
        return requestGET(apiPath, response);
    }

    // DELETE /api/recorded/:id
    int deleteRecordedProgram(std::string id)
    {
        const std::string apiPath = "recorded/" + id;
        return requestDELETE(apiPath);
    }

    // DELETE /api/reserves/:id
    int deleteReserves(std::string id)
    {
        const std::string apiPath = "reserves/" + id;
        return requestDELETE(apiPath);
    }

    // DELETE /api/reserves/:id/skip
    int deleteReservesSkip(std::string id)
    {
        const std::string apiPath = "reserves/" + id + "/skip";
        return requestDELETE(apiPath);
    }

    // POST /api/reserves
    int postReserves(std::string id)
    {
        std::string buffer = "{\"programId\":" + id + ",\"allowEndLack\":true}\"_method\":\"POST\"}";
        const std::string apiPath = "reserves";
        return requestPOST(apiPath, buffer.c_str(), buffer.size());
    }

    // GET /api/rules
    int getRules(picojson::value& response)
    {
        const std::string apiPath = "rules";
        return requestGET(apiPath, response);
    }

    // FIXME: Apply other arguments
    // POST /api/rules
    int postRules(std::string type, std::string channel, std::string title, std::string genre)
    {
        const std::string apiPath = "rules";
        std::string buffer = "{\"search\":{\"week\":127,\"keyword\":\"" + title + "\",\"title\":true,\"description\":true,\"GR\":true,\"BS\":true,\"CS\":"
                                                                                  "true,\"SKY\":true},\"option\":{\"enable\":true,\"allowEndLack\":true}}";
        return requestPOST(apiPath, buffer.c_str(), buffer.size());
    }

    // PUT /api/rules/:id/:action
    int putRuleAction(int id, bool state)
    {
        const std::string apiPath = "rules/" + std::to_string(id) + (state ? "/enable" : "/disable");
        return requestPUT(apiPath);
    }

    // PUT /api/schedule/update
    int putScheduleUpdate()
    {
        const std::string apiPath = "schedule/update";
        return requestPUT(apiPath);
    }

    // GET /api/storage
    int getStorage(picojson::value& response)
    {
        const std::string apiPath = "storage";
        return requestGET(apiPath, response);
    }

} // namespace api
} // namespace epgstation
