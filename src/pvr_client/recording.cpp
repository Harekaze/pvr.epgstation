/*
 * Copyright (C) 2015-2020 Yuki MIZUNO
 * https://github.com/Harekaze/pvr.epgstation/
 * SPDX-License-Identifier: GPL-3.0-only
 */
#include <iostream>
#include <climits>
#include "kodi/libKODI_guilib.h"
#include "kodi/libXBMC_addon.h"
#include "kodi/libXBMC_pvr.h"
#include "epgstation/epgstation.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(sec) Sleep(sec)
#else
#include <unistd.h>
#endif

extern epgstation::Recorded g_recorded;
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr *PVR;

extern "C" {

int GetRecordingsAmount(bool deleted) {
	return g_recorded.programs.size();
}

PVR_ERROR GetRecordings(ADDON_HANDLE handle, bool deleted) {
	if (g_recorded.refresh()) {
		for (const PVR_RECORDING rec: g_recorded.programs) {
			PVR->TransferRecordingEntry(handle, &rec);
		}
		return PVR_ERROR_NO_ERROR;
	}
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetRecordingStreamProperties(const PVR_RECORDING* recording, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount) {
	strncpy(properties[0].strName, PVR_STREAM_PROPERTY_STREAMURL, sizeof(properties[0].strName) - 1);
	snprintf(properties[0].strValue, sizeof(properties[0].strValue) - 1, (const char*)(epgstation::api::baseURL + g_recorded.recordedStreamingPath).c_str(), recording->strRecordingId);
	*iPropertiesCount = 1;
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING &recording) {
	if (epgstation::api::deleteRecordedProgram(recording.strRecordingId) != epgstation::api::REQUEST_FAILED) {
		XBMC->Log(ADDON::LOG_NOTICE, "Delete recording: %s", recording.strRecordingId);
		sleep(1);
		PVR->TriggerRecordingUpdate();
		return PVR_ERROR_NO_ERROR;
	}
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetDriveSpace(long long *iTotal, long long *iUsed) {
	picojson::value response;
	const time_t refreshInterval = 10*60; // every 10 minutes
	static time_t lastUpdated;
	static long long total, used;

	time_t now;
	time(&now);

	if (now - lastUpdated < refreshInterval) {
		*iTotal = total;
		*iUsed = used;
		return PVR_ERROR_NO_ERROR;
	}

	if (epgstation::api::getStorage(response) == epgstation::api::REQUEST_FAILED) {
		return PVR_ERROR_SERVER_ERROR;
	}

	picojson::object &o = response.get<picojson::object>();
	total = (long long)(o["size"].get<double>() / 1024);
	used = (long long)(o["used"].get<double>() / 1024);
	*iTotal = total;
	*iUsed = used;

	lastUpdated = now;
	return PVR_ERROR_NO_ERROR;
}

/* not implemented */
PVR_ERROR UndeleteRecording(const PVR_RECORDING& recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR DeleteAllRecordingsFromTrash(void) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR RenameRecording(const PVR_RECORDING &recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLifetime(const PVR_RECORDING* recording) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingPlayCount(const PVR_RECORDING &recording, int count) { return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING &recording, int lastplayedposition) { return PVR_ERROR_NOT_IMPLEMENTED; }
int GetRecordingLastPlayedPosition(const PVR_RECORDING &recording) { return -1; }
PVR_ERROR GetRecordingEdl(const PVR_RECORDING&, PVR_EDL_ENTRY[], int*) { return PVR_ERROR_NOT_IMPLEMENTED; };
bool OpenRecordedStream(const PVR_RECORDING &recording) { return false; }
void CloseRecordedStream(void) {}
int ReadRecordedStream(unsigned char *pBuffer, unsigned int iBufferSize) { return 0; }
long long SeekRecordedStream(long long iPosition, int iWhence) { return 0; }
long long LengthRecordedStream(void) { return 0; }

}
