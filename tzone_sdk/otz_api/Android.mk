LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug eng

LOCAL_MODULE := libotzapi
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_LIB)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SRC_FILES += otz_api.c
LOCAL_SRC_FILES += otz_tee_api.c

include $(BUILD_SHARED_LIBRARY)
