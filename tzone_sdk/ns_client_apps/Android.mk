LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug eng

LOCAL_MODULE := otzapp
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SHARED_LIBRARIES := \
	libotzapi 

LOCAL_SRC_FILES += otzapp.c

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug eng

LOCAL_MODULE := otz_virtual_keyboard
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SHARED_LIBRARIES := \
	libotzapi

LOCAL_SRC_FILES += otz_virtual_keyboard.c

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug eng

LOCAL_MODULE := otz_boot_ack
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SHARED_LIBRARIES := \
	libotzapi \

LOCAL_SRC_FILES += otz_boot_ack.c

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug eng

LOCAL_MODULE := otz_tee_app
#LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	external/trustzone/tzone_sdk/include

LOCAL_SHARED_LIBRARIES := \
	libotzapi 

LOCAL_SRC_FILES += otz_tee_app.c

include $(BUILD_EXECUTABLE)
