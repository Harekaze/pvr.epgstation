/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/epgstation.h"
#include "epgstation/genre.h"
#include "epgstation/types.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include <climits>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

constexpr int CREATE_TIMER_MANUAL_RESERVED = 0x11;
constexpr int CREATE_RULES_PATTERN_MATCHED = 0x12;
constexpr unsigned int TIMER_MANUAL_RESERVED = 0x01;
constexpr unsigned int TIMER_PATTERN_MATCHED = 0x02;

#define MSG_TIMER_MANUAL_RESERVED 30900
#define MSG_TIMER_PATTERN_MATCHED 30901
#define MSG_RULES_PATTERN_MATCHED_CREATION 30902

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;
extern epgstation::Recorded g_recorded;
extern epgstation::Schedule g_schedule;
extern epgstation::Rule g_rule;
extern epgstation::Reserve g_reserve;
extern epgstation::Channels g_channels;

struct tm* localtime_now()
{
    time_t now;
    time(&now);
#if defined(_WIN32) || defined(_WIN64)
    return localtime(&now); // NOLINT
#else
    struct tm time;
    return localtime_r(&now, &time);
#endif
}

uint8_t get_hour(time_t time)
{
#if defined(_WIN32) || defined(_WIN64)
    return localtime(&time)->tm_hour; // NOLINT
#else
    struct tm t;
    return localtime_r(&time, &t)->tm_hour;
#endif
}

extern "C" {

int GetTimersAmount(void)
{
    if (g_rule.refresh() && g_reserve.refresh()) {
        return g_rule.rules.size() + g_reserve.reserves.size();
    }
    return -1;
}

PVR_ERROR GetTimers(ADDON_HANDLE handle)
{
    if (g_rule.refresh() && g_reserve.refresh()) {
        for (const auto rule : g_rule.rules) {
            PVR_TIMER timer = {
                .iClientIndex = static_cast<unsigned int>(rule.id),
                .iParentClientIndex = PVR_TIMER_NO_PARENT,
                .iClientChannelUid = rule.station == 0 ? PVR_TIMER_ANY_CHANNEL : g_channels.getId(rule.station),
                .startTime = 0,
                .endTime = 0,
                .bStartAnyTime = rule.timeRange == 0,
                .bEndAnyTime = rule.timeRange == 0,
                .state = rule.enable ? PVR_TIMER_STATE_SCHEDULED : PVR_TIMER_STATE_DISABLED,
                .iTimerType = CREATE_RULES_PATTERN_MATCHED,
            };
            timer.iWeekdays = rule.week;
            timer.bFullTextEpgSearch = rule.description;
            strncpy(timer.strTitle, rule.keyword.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            if (!timer.bStartAnyTime) {
                auto time = localtime_now();
                time->tm_hour = rule.startTime;
                time->tm_min = 0;
                timer.startTime = mktime(time);
                timer.endTime = timer.startTime + rule.timeRange * 60 * 60;
            }
            strncpy(timer.strEpgSearchString, rule.keyword.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(timer.strSummary, rule.keyword.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(timer.strDirectory, rule.directory.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);

            PVR->TransferTimerEntry(handle, &timer);
        }

        for (const auto p : g_reserve.reserves) {
            struct PVR_TIMER timer = {
                .iClientIndex = static_cast<unsigned int>(p.id),
                .iParentClientIndex = static_cast<unsigned int>(p.ruleId),
                .iClientChannelUid = g_channels.getId(p.channelId),
                .startTime = p.startAt,
                .endTime = p.endAt,
                .bStartAnyTime = false,
                .bEndAnyTime = false,
                .state = PVR_TIMER_STATE_NEW,
                .iTimerType = p.ruleId != 0 ? TIMER_PATTERN_MATCHED : TIMER_MANUAL_RESERVED,
            };
            const auto genre = epgstation::getGenreCodeFromContentNibble(p.genre1, p.genre2);
            timer.iGenreType = genre.main;
            timer.iGenreSubType = genre.sub;
            timer.iEpgUid = static_cast<unsigned int>(p.eventId);

            strncpy(timer.strTitle, p.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(timer.strSummary, (p.extended + p.description).c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(timer.strDirectory, std::to_string(p.id).c_str(), PVR_ADDON_URL_STRING_LENGTH - 1); // NOTE: Store original ID

            switch (p.state) {
            case epgstation::STATE_RESERVED:
                timer.state = PVR_TIMER_STATE_SCHEDULED;
                break;
            case epgstation::STATE_CONFLICT:
                timer.state = PVR_TIMER_STATE_CONFLICT_NOK;
                break;
            case epgstation::STATE_SKIPPED:
                timer.state = PVR_TIMER_STATE_DISABLED;
                break;
            }

            PVR->TransferTimerEntry(handle, &timer);
        }

        return PVR_ERROR_NO_ERROR;
    }

    return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR UpdateTimer(const PVR_TIMER& timer)
{
    switch (timer.iTimerType) {
    case CREATE_RULES_PATTERN_MATCHED: {
        const auto rule = std::find_if(g_rule.rules.begin(), g_rule.rules.end(), [timer](epgstation::rule r) {
            return r.id == timer.iClientIndex;
        });
        if (rule == g_rule.rules.end()) {
            XBMC->Log(ADDON::LOG_ERROR, "Rule not found: #%d", timer.iClientIndex);
            return PVR_ERROR_REJECTED;
        }

        if (timer.state == (rule->enable ? PVR_TIMER_STATE_SCHEDULED : PVR_TIMER_STATE_DISABLED)) {
            // Timer state is not changed. Update rule
            auto startHour = get_hour(timer.startTime);
            auto endHour = get_hour(timer.endTime);
            if (g_rule.edit(timer.iClientIndex, timer.state != PVR_TIMER_STATE_DISABLED,
                    timer.strEpgSearchString, timer.bFullTextEpgSearch,
                    g_channels.getId(timer.iClientChannelUid), timer.iWeekdays, startHour, endHour,
                    timer.bStartAnyTime || timer.bEndAnyTime, timer.strDirectory)) {
                goto complete;
            }
            return PVR_ERROR_SERVER_ERROR;
        } else {
            switch (timer.state) {
            case PVR_TIMER_STATE_SCHEDULED:
            case PVR_TIMER_STATE_DISABLED: {
                bool enable = timer.state == PVR_TIMER_STATE_SCHEDULED;
                if (g_rule.enable(timer.iClientIndex, enable)) {
                    goto complete;
                }
                return PVR_ERROR_SERVER_ERROR;
            }
            default:
                XBMC->Log(ADDON::LOG_ERROR, "Unknown state change: #%d", timer.iClientIndex);
                return PVR_ERROR_NOT_IMPLEMENTED;
            }
        }
    }
    case TIMER_PATTERN_MATCHED: {
        switch (timer.state) {
        case PVR_TIMER_STATE_SCHEDULED:
            if (g_reserve.restore(timer.strDirectory)) {
                goto complete;
            }
            return PVR_ERROR_SERVER_ERROR;
        case PVR_TIMER_STATE_DISABLED:
            if (g_reserve.remove(timer.strDirectory)) {
                goto complete;
            }
            return PVR_ERROR_SERVER_ERROR;
        default:
            XBMC->Log(ADDON::LOG_ERROR, "Unknown state change: %s", timer.strDirectory);
            return PVR_ERROR_NOT_IMPLEMENTED;
        }
    }
    default: {
        XBMC->Log(ADDON::LOG_ERROR, "Unknown iTimerType: %d", timer.iTimerType);
        return PVR_ERROR_NOT_IMPLEMENTED;
    }
    }

complete:
    sleep(1);
    PVR->TriggerTimerUpdate();
    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR AddTimer(const PVR_TIMER& timer)
{
    switch (timer.iTimerType) {
    case CREATE_RULES_PATTERN_MATCHED: {
        auto startHour = get_hour(timer.startTime);
        auto endHour = get_hour(timer.endTime);

        if (g_rule.add(timer.state != PVR_TIMER_STATE_DISABLED, timer.strEpgSearchString, timer.bFullTextEpgSearch,
                g_channels.getId(timer.iClientChannelUid), timer.iWeekdays, startHour, endHour,
                timer.bStartAnyTime || timer.bEndAnyTime, timer.strDirectory)) {
            goto complete;
        }
        return PVR_ERROR_SERVER_ERROR;
    }
    case CREATE_TIMER_MANUAL_RESERVED: {
        auto programs = g_schedule.list[g_channels.getId(timer.iClientChannelUid)];
        if (programs.empty()) {
            programs = g_schedule.fetch(g_channels.getId(timer.iClientChannelUid), timer.startTime, timer.endTime);
        }

        const auto program = std::find_if(programs.begin(), programs.end(), [timer](epgstation::program p) {
            return p.startAt == timer.startTime;
        });
        if (program != programs.end()) {
            if (g_reserve.add(std::to_string(program->id))) {
                goto complete;
            }
            return PVR_ERROR_SERVER_ERROR;
        }
    }
    }
    XBMC->Log(ADDON::LOG_ERROR, "Failed to reserve new program: nothing matched");
    return PVR_ERROR_FAILED;

complete:
    sleep(3);
    PVR->TriggerTimerUpdate();
    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DeleteTimer(const PVR_TIMER& timer, bool bForceDelete)
{
    switch (timer.iTimerType) {
    case TIMER_MANUAL_RESERVED: {
        if (g_reserve.remove(timer.strDirectory)) {
            sleep(1);
            PVR->TriggerRecordingUpdate();
            PVR->TriggerTimerUpdate();
            return PVR_ERROR_NO_ERROR;
        }
        return PVR_ERROR_SERVER_ERROR;
    }
    case CREATE_RULES_PATTERN_MATCHED: {
        if (g_rule.remove(timer.iClientIndex)) {
            sleep(1);
            PVR->TriggerRecordingUpdate();
            PVR->TriggerTimerUpdate();
            return PVR_ERROR_NO_ERROR;
        }
        return PVR_ERROR_SERVER_ERROR;
    }
    default: {
        XBMC->Log(ADDON::LOG_ERROR, "Unknown timer type for deletion request: %d", timer.iTimerType);
        return PVR_ERROR_NOT_IMPLEMENTED;
    }
    }
}

PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int* size)
{
    std::vector<PVR_TIMER_TYPE> tt = {
        {
            .iId = CREATE_RULES_PATTERN_MATCHED,
            .iAttributes = PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE
                | PVR_TIMER_TYPE_SUPPORTS_TITLE_EPG_MATCH | PVR_TIMER_TYPE_SUPPORTS_FULLTEXT_EPG_MATCH
                | PVR_TIMER_TYPE_SUPPORTS_CHANNELS | PVR_TIMER_TYPE_SUPPORTS_ANY_CHANNEL
                | PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_START_ANYTIME
                | PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_END_ANYTIME
                | PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS | PVR_TIMER_TYPE_IS_REPEATING | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS,
        },
        {
            .iId = CREATE_TIMER_MANUAL_RESERVED,
            .iAttributes = PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE,
        },
        {
            .iId = TIMER_MANUAL_RESERVED,
            .iAttributes = PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES | PVR_TIMER_TYPE_IS_READONLY | PVR_TIMER_TYPE_IS_MANUAL
                | PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_READONLY_DELETE,
        },
        {
            .iId = TIMER_PATTERN_MATCHED,
            .iAttributes = PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES | PVR_TIMER_TYPE_IS_READONLY | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE,
        },
    };

    strncpy(std::find_if(tt.begin(), tt.end(), [](PVR_TIMER_TYPE t) { return t.iId == CREATE_RULES_PATTERN_MATCHED; })->strDescription,
        XBMC->GetLocalizedString(MSG_RULES_PATTERN_MATCHED_CREATION), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
    strncpy(std::find_if(tt.begin(), tt.end(), [](PVR_TIMER_TYPE t) { return t.iId == TIMER_MANUAL_RESERVED; })->strDescription,
        XBMC->GetLocalizedString(MSG_TIMER_MANUAL_RESERVED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
    strncpy(std::find_if(tt.begin(), tt.end(), [](PVR_TIMER_TYPE t) { return t.iId == TIMER_PATTERN_MATCHED; })->strDescription,
        XBMC->GetLocalizedString(MSG_TIMER_PATTERN_MATCHED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);

    std::copy(tt.begin(), tt.end(), types);
    *size = tt.size();

    return PVR_ERROR_NO_ERROR;
}

/* not implemented */
bool IsTimeshifting() { return false; }
}
