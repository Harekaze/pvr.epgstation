/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include "epgstation/epgstation.h"
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include <iostream>
#include <set>

extern epgstation::Channels g_channels;
extern ADDON::CHelper_libXBMC_addon* XBMC;
extern CHelper_libXBMC_pvr* PVR;

extern "C" {

int GetChannelsAmount(void)
{
    if (!g_channels.refresh()) {
        return PVR_ERROR_SERVER_ERROR;
    }
    return g_channels.channels.size();
}

PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio)
{
    if (bRadio) {
        return PVR_ERROR_NO_ERROR;
    }

    if (!g_channels.refresh()) {
        return PVR_ERROR_SERVER_ERROR;
    }

    std::vector<std::string> transferred;

    for (const auto c : g_channels.channels) {
        PVR_CHANNEL ch = {
            .iUniqueId = static_cast<unsigned int>(g_channels.getId(c.id)),
            .bIsRadio = false,
            .iChannelNumber = c.remoteControlKeyId,
            .iSubChannelNumber = c.serviceId,
        };

        if (std::find(transferred.begin(), transferred.end(), c.channelStr) != transferred.end()) {
            XBMC->Log(ADDON::LOG_DEBUG, "Hide \"%s\" because main channel of %s is transferred", c.name.c_str(), c.channelStr.c_str());
            ch.bIsHidden = true;
        }

        strncpy(ch.strChannelName, c.name.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);

        if (c.hasLogoData) {
            snprintf(ch.strIconPath, PVR_ADDON_URL_STRING_LENGTH - 1, g_channels.channelLogoPath.c_str(), c.id);
        }
        PVR->TransferChannelEntry(handle, &ch);

        transferred.push_back(c.channelStr);
    }

    return PVR_ERROR_NO_ERROR;
}

int GetChannelGroupsAmount(void)
{
    std::set<std::string> list;
    for (const auto channel : g_channels.channels) {
        list.insert(channel.channelType);
    }
    return list.size();
}

PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio)
{
    std::set<std::string> list;
    for (const auto channel : g_channels.channels) {
        list.insert(channel.channelType);
    }

    for (const auto channelType : list) {
        PVR_CHANNEL_GROUP chGroup = {};
        strncpy(chGroup.strGroupName, channelType.c_str(), PVR_ADDON_NAME_STRING_LENGTH - 1);

        PVR->TransferChannelGroup(handle, &chGroup);
    }

    return PVR_ERROR_NO_ERROR;
}

PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP& group)
{
    for (const auto c : g_channels.channels) {
        if (c.channelType != group.strGroupName) {
            continue;
        }
        PVR_CHANNEL_GROUP_MEMBER chMem = {};
        chMem.iChannelUniqueId = static_cast<unsigned int>(g_channels.getId(c.id));
        chMem.iChannelNumber = c.remoteControlKeyId;
        strncpy(chMem.strGroupName, group.strGroupName, PVR_ADDON_NAME_STRING_LENGTH - 1);

        PVR->TransferChannelGroupMember(handle, &chMem);
    }

    return PVR_ERROR_NO_ERROR;
}

/* not implemented */
PVR_ERROR OpenDialogChannelScan(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteChannel(const PVR_CHANNEL& channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameChannel(const PVR_CHANNEL& channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelSettings(const PVR_CHANNEL& channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR OpenDialogChannelAdd(const PVR_CHANNEL& channel) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetEPGTimeFrame(int iDays) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR GetChannelStreamProperties(const PVR_CHANNEL* channel, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount) { return PVR_ERROR_NOT_IMPLEMENTED; }
}
