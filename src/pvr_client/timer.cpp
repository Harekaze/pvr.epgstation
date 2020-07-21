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

#define RULE_CLIENT_START_INDEX 0x100

extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;
extern epgstation::Recorded g_recorded;
extern epgstation::Schedule g_schedule;
extern epgstation::Rule g_rule;
extern epgstation::Reserve g_reserve;

extern "C" {

int GetTimersAmount(void)
{
    return g_rule.rules.size() + g_reserve.reserves.size();
}

PVR_ERROR GetTimers(ADDON_HANDLE handle)
{
    if (g_rule.refresh() && g_reserve.refresh()) {
        unsigned int index = 0;
        time_t now;
        time(&now);

        for (const auto rule : g_rule.rules) {
            PVR_TIMER timer;
            memset(&timer, 0, sizeof(PVR_TIMER));

            timer.iClientIndex = rule.id + RULE_CLIENT_START_INDEX;
            timer.state = rule.enable ? PVR_TIMER_STATE_SCHEDULED : PVR_TIMER_STATE_DISABLED;
            strncpy(timer.strTitle, rule.keyword.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            timer.iClientChannelUid = rule.station == -1 ? PVR_TIMER_ANY_CHANNEL : rule.station;
            timer.iTimerType = CREATE_RULES_PATTERN_MATCHED;
            timer.bStartAnyTime = rule.timeRange == 0;
            timer.bEndAnyTime = rule.timeRange == 0;
            if (!timer.bStartAnyTime) {
                struct tm* time = localtime(&now);
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

            timer.iEpgUid = p.id; // NOTE: Overflow casting from unsigned long to unsigned
            timer.iClientIndex = index++;
            timer.iClientChannelUid = p.channelId;
            strncpy(timer.strTitle, p.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);
            strncpy(timer.strSummary, p.extended.c_str(), PVR_ADDON_DESC_STRING_LENGTH - 1);
            strncpy(timer.strDirectory, std::to_string(p.id).c_str(), PVR_ADDON_URL_STRING_LENGTH - 1); // NOTE: Store original ID

            timer.state = PVR_TIMER_STATE_SCHEDULED;
            timer.startTime = p.startAt;
            timer.endTime = p.endAt;
            timer.iGenreType = genre & epgstation::GENRE_TYPE_MASK;
            timer.iGenreSubType = genre & epgstation::GENRE_SUBTYPE_MASK;
            timer.bStartAnyTime = false;
            timer.bEndAnyTime = false;
            timer.iTimerType = p.ruleId != -1 ? TIMER_PATTERN_MATCHED : TIMER_MANUAL_RESERVED;
            if (p.ruleId != -1) {
                timer.iParentClientIndex = p.ruleId + RULE_CLIENT_START_INDEX;
            }

            if (now < timer.startTime) {
                timer.state = PVR_TIMER_STATE_SCHEDULED;
            } else if (timer.startTime < now && now < timer.endTime) {
                timer.state = PVR_TIMER_STATE_RECORDING;
            } else {
                timer.state = PVR_TIMER_STATE_COMPLETED;
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
        const unsigned int index = timer.iClientIndex - RULE_CLIENT_START_INDEX;
        epgstation::rule* rule = NULL;
        for (auto& r : g_rule.rules) {
            if (r.id == index) {
                rule = &r;
                break;
            }
        }
        if (rule == NULL) {
            XBMC->Log(ADDON::LOG_ERROR, "Rule not found: #%d", index);
            return PVR_ERROR_REJECTED;
        }

        if (timer.state == rule->enable ? PVR_TIMER_STATE_SCHEDULED : PVR_TIMER_STATE_DISABLED) {
            // Timer state is not changed. Update rule
            unsigned int startHour = localtime(&timer.startTime)->tm_hour;
            unsigned int endHour = localtime(&timer.endTime)->tm_hour;

            if (epgstation::api::putRule(index, timer.state != PVR_TIMER_STATE_DISABLED,
                    timer.strEpgSearchString, timer.bFullTextEpgSearch,
                    timer.iClientChannelUid, timer.iWeekdays, startHour, endHour,
                    timer.bStartAnyTime || timer.bEndAnyTime, timer.strDirectory)
                != epgstation::api::REQUEST_FAILED) {
                XBMC->Log(ADDON::LOG_NOTICE, "Update rule: %d", index);
                goto complete;
            }
            XBMC->Log(ADDON::LOG_ERROR, "Failed to update rule: %d", index);
            return PVR_ERROR_SERVER_ERROR;
        } else {
            switch (timer.state) {
            case PVR_TIMER_STATE_SCHEDULED:
            case PVR_TIMER_STATE_DISABLED: {
                bool enable = timer.state == PVR_TIMER_STATE_SCHEDULED;
                if (epgstation::api::putRuleAction(rule->id, enable) != epgstation::api::REQUEST_FAILED) {
                    XBMC->Log(ADDON::LOG_NOTICE, "%s rule: #%d", enable ? "Enable" : "Disable", index);
                    goto complete;
                }
                XBMC->Log(ADDON::LOG_ERROR, "Failed to %s rule: #%d", enable ? "enable" : "disable", index);
                return PVR_ERROR_SERVER_ERROR;
            }
            default:
                XBMC->Log(ADDON::LOG_ERROR, "Unknown state change: #%d", index);
                return PVR_ERROR_NOT_IMPLEMENTED;
            }
        }
    }
    case TIMER_PATTERN_MATCHED: {
        switch (timer.state) {
        case PVR_TIMER_STATE_SCHEDULED:
            if (epgstation::api::deleteReservesSkip(timer.strDirectory) != epgstation::api::REQUEST_FAILED) {
                XBMC->Log(ADDON::LOG_NOTICE, "Unskip reserving: %s", timer.strDirectory);
                goto complete;
            }
            XBMC->Log(ADDON::LOG_ERROR, "Failed to enable state: %s", timer.strDirectory);
            return PVR_ERROR_SERVER_ERROR;
        case PVR_TIMER_STATE_DISABLED:
            if (epgstation::api::deleteReserves(timer.strDirectory) != epgstation::api::REQUEST_FAILED) {
                XBMC->Log(ADDON::LOG_NOTICE, "Skip reserving: %s", timer.strDirectory);
                goto complete;
            }
            XBMC->Log(ADDON::LOG_ERROR, "Failed to disable state: %s", timer.strDirectory);
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
        unsigned int startHour = localtime(&timer.startTime)->tm_hour;
        unsigned int endHour = localtime(&timer.endTime)->tm_hour;

        if (epgstation::api::postRules(timer.state != PVR_TIMER_STATE_DISABLED, timer.strEpgSearchString, timer.bFullTextEpgSearch,
                timer.iClientChannelUid, timer.iWeekdays, startHour, endHour,
                timer.bStartAnyTime || timer.bEndAnyTime, timer.strDirectory)
            != epgstation::api::REQUEST_FAILED) {
            XBMC->Log(ADDON::LOG_NOTICE, "Create new rule: \"%s\"", timer.strEpgSearchString);
            goto complete;
        }
        XBMC->Log(ADDON::LOG_ERROR, "Failed to create new rule: %s", timer.strEpgSearchString);
        return PVR_ERROR_SERVER_ERROR;
    }
    case CREATE_TIMER_MANUAL_RESERVED: {
        for (const auto program : g_schedule.programs) {
            if ((int)program.channelId == timer.iClientChannelUid && program.startAt == timer.startTime && program.endAt == timer.endTime) {
                if (epgstation::api::postReserves(std::to_string(program.id)) != epgstation::api::REQUEST_FAILED) {
                    XBMC->Log(ADDON::LOG_NOTICE, "Reserved new program: %s", std::to_string(program.id).c_str());
                    goto complete;
                }
                XBMC->Log(ADDON::LOG_ERROR, "Failed to reserve new program: %s", std::to_string(program.id).c_str());
                return PVR_ERROR_SERVER_ERROR;
            }
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
        for (const auto program : g_schedule.programs) {
            if ((int)program.channelId == timer.iClientChannelUid && program.startAt == timer.startTime && program.endAt == timer.endTime) {
                if (epgstation::api::deleteReserves(timer.strDirectory) != epgstation::api::REQUEST_FAILED) {
                    XBMC->Log(ADDON::LOG_NOTICE, "Delete reserved program: %s", timer.strDirectory);
                    sleep(1);
                    PVR->TriggerRecordingUpdate();
                    PVR->TriggerTimerUpdate();
                    return PVR_ERROR_NO_ERROR;
                }
                XBMC->Log(ADDON::LOG_ERROR, "Failed to delete reserved program: %s", std::to_string(program.id).c_str());
                return PVR_ERROR_SERVER_ERROR;
            }
        }
        XBMC->Log(ADDON::LOG_ERROR, "Failed to delete timer: nothing matched");
        return PVR_ERROR_FAILED;
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
