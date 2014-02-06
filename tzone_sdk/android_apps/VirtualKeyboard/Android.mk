LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := VirtualKeyboard

LOCAL_JNI_SHARED_LIBRARIES := libvirtualkeyboard

LOCAL_REQUIRED_MODULES := libvirtualkeyboard

include $(BUILD_PACKAGE)

include $(call all-makefiles-under, $(LOCAL_PATH))
