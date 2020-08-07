LOCAL_PATH := $(call my-dir)/..

## epgstation
include $(CLEAR_VARS)

LOCAL_MODULE      := epgstation

LOCAL_SRC_FILES   := $(wildcard src/epgstation/*.cpp) $(wildcard src/base64/*.cpp)
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/include $(LOCAL_PATH)/src

include $(BUILD_STATIC_LIBRARY)

## pvr_client
include $(CLEAR_VARS)

LOCAL_MODULE      := pvr.epgstation

LOCAL_SRC_FILES   := $(wildcard src/pvr_client/*.cpp)
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/include $(LOCAL_PATH)/src
LOCAL_STATIC_LIBRARIES := epgstation

include $(BUILD_SHARED_LIBRARY)
