LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng

LOCAL_MODULE := libtest

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SHARED_LIBRARIES := \
	libotzapi 

LOCAL_SRC_FILES := otz_tee_app.c \
	func2.c

include $(BUILD_SHARED_LIBRARY)
