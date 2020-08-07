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

#define CREATE_TIMER_MANUAL_RESERVED 0x11
#define CREATE_RULES_PATTERN_MATCHED 0x12

#define MSG_TIMER_MANUAL_RESERVED 30900
#define MSG_TIMER_PATTERN_MATCHED 30901
#define MSG_RULES_PATTERN_MATCHED_CREATION 30902

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;
extern epgstation::Recorded g_recorded;
extern epgstation::Schedule g_schedule;
extern epgstation::Rule g_rule;
extern epgstation::Reserve g_reserve;

struct tm* localtime_now()
{
    time_t now;
    time(&now);
#if defined(_WIN32) || defined(_WIN64)
    return localtime(&now);
#else
    struct tm time;
    return localtime_r(&now, &time);
#endif
}

uint8_t get_hour(time_t time)
{
#if defined(_WIN32) || defined(_WIN64)
    return localtime(&time)->tm_hour;
#else
    struct tm t;
    return localtime_r(&time, &t)->tm_hour;
#endif
}

extern "C" {

int GetTimersAmount(void)
{
    return g_rule.rules.size() + g_reserve.reserves.size();
}

PVR_ERROR GetTimers(ADDON_HANDLE handle)
{
    if (g_rule.refresh() && g_reserve.refresh()) {
        for (const auto rule : g_rule.rules) {
            PVR_TIMER timer;
            memset(&timer, 0, sizeof(PVR_TIMER));

            timer.iClientIndex = rule.id;
            timer.state = rule.enable ? PVR_TIMER_STATE_SCHEDULED : PVR_TIMER_STATE_DISABLED;
            strncpy(timer.strTitle, rule.keyword.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            timer.iClientChannelUid = rule.station == -1 ? PVR_TIMER_ANY_CHANNEL : rule.station;
            timer.iTimerType = CREATE_RULES_PATTERN_MATCHED;
            timer.bStartAnyTime = rule.timeRange == 0;
            timer.bEndAnyTime = rule.timeRange == 0;
            if (!timer.bStartAnyTime) {
                auto time = localtime_now();
                time->tm_hour = rule.startTime;
                time->tm_min = 0;
                timer.startTime = mktime(time);
                timer.endTime = timer.startTime + rule.timeRange * 60 * 60;
            }
            timer.iWeekdays = rule.week;
            strncpy(timer.strEpgSearchString, rule.keyword.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(timer.strSummary, rule.keyword.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(timer.strDirectory, rule.directory.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            timer.bFullTextEpgSearch = rule.description;

            PVR->TransferTimerEntry(handle, &timer);
        }

        for (const auto p : g_reserve.reserves) {
            const auto genre = epgstation::getGenreCodeFromContentNibble(p.genre1, p.genre2);
            struct PVR_TIMER timer;
            memset(&timer, 0, sizeof(PVR_TIMER));

            timer.iEpgUid = p.eventId;
            timer.iClientIndex = p.id;
            timer.iClientChannelUid = p.channelId;
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

            timer.startTime = p.startAt;
            timer.endTime = p.endAt;
            timer.iGenreType = genre.main;
            timer.iGenreSubType = genre.sub;
            timer.bStartAnyTime = false;
            timer.bEndAnyTime = false;
            timer.iTimerType = p.ruleId != -1 ? TIMER_PATTERN_MATCHED : TIMER_MANUAL_RESERVED;
            if (p.ruleId != -1) {
                timer.iParentClientIndex = p.ruleId;
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
                    timer.iClientChannelUid, timer.iWeekdays, startHour, endHour,
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
                timer.iClientChannelUid, timer.iWeekdays, startHour, endHour,
                timer.bStartAnyTime || timer.bEndAnyTime, timer.strDirectory)) {
            goto complete;
        }
        return PVR_ERROR_SERVER_ERROR;
    }
    case CREATE_TIMER_MANUAL_RESERVED: {
        const auto program = std::find_if(g_schedule.programs.begin(), g_schedule.programs.end(), [timer](epgstation::program p) {
            return static_cast<int>(p.channelId) == timer.iClientChannelUid && p.startAt == timer.startTime;
        });
        if (program != g_schedule.programs.end()) {
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
    default: {
        XBMC->Log(ADDON::LOG_ERROR, "Unknown timer type for deletion request: %d", timer.iTimerType);
        return PVR_ERROR_NOT_IMPLEMENTED;
    }
    }
}

PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int* size)
{
    int& count = *size;
    count = 0;

    // Create timer rule
    memset(&types[count], 0, sizeof(types[count]));
    types[count].iId = CREATE_RULES_PATTERN_MATCHED;
    types[count].iAttributes = PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE
        | PVR_TIMER_TYPE_SUPPORTS_TITLE_EPG_MATCH | PVR_TIMER_TYPE_SUPPORTS_FULLTEXT_EPG_MATCH
        | PVR_TIMER_TYPE_SUPPORTS_CHANNELS | PVR_TIMER_TYPE_SUPPORTS_ANY_CHANNEL
        | PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_START_ANYTIME
        | PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_END_ANYTIME
        | PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS | PVR_TIMER_TYPE_IS_REPEATING | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
    strncpy(types[count].strDescription, XBMC->GetLocalizedString(MSG_RULES_PATTERN_MATCHED_CREATION), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
    count++;

    // Reserve program manually
    memset(&types[count], 0, sizeof(types[count]));
    types[count].iId = CREATE_TIMER_MANUAL_RESERVED;
    types[count].iAttributes = PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE;
    count++;

    // Manual reserved programs
    memset(&types[count], 0, sizeof(types[count]));
    types[count].iId = TIMER_MANUAL_RESERVED;
    types[count].iAttributes = PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES | PVR_TIMER_TYPE_IS_READONLY | PVR_TIMER_TYPE_IS_MANUAL
        | PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_READONLY_DELETE;
    strncpy(types[count].strDescription, XBMC->GetLocalizedString(MSG_TIMER_MANUAL_RESERVED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
    count++;

    // Rule based reserved programs
    memset(&types[count], 0, sizeof(types[count]));
    types[count].iId = TIMER_PATTERN_MATCHED;
    types[count].iAttributes = PVR_TIMER_TYPE_FORBIDS_NEW_INSTANCES | PVR_TIMER_TYPE_IS_READONLY | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE;
    strncpy(types[count].strDescription, XBMC->GetLocalizedString(MSG_TIMER_PATTERN_MATCHED), PVR_ADDON_TIMERTYPE_STRING_LENGTH - 1);
    count++;

    return PVR_ERROR_NO_ERROR;
}

/* not implemented */
bool IsTimeshifting() { return false; }
}
