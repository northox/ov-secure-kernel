LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug eng
LOCAL_MODULE    := libSwDrmPlugin
#LOCAL_MODULE_PATH := $(SYSROOT)/lib/drm/
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/drm/
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := swDrmPluginMain.cpp \
			otz_drm_app.c

LOCAL_SHARED_LIBRARIES := liblog libutils libotzapi

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v4

LOCAL_STATIC_LIBRARIES := libdrmutility libdrmframeworkcommon
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	frameworks/base/drm/libdrmframework/include \
	frameworks/base/drm/libdrmframework/plugins/common/include \
	frameworks/base/include \
	system/core/include \
	frameworks/base/include \
	external/trustzone/tzone_sdk/include

include $(BUILD_SHARED_LIBRARY)
