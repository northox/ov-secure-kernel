LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := TestDrm

LOCAL_JNI_SHARED_LIBRARIES := libSwDrmPlugin

LOCAL_REQUIRED_MODULES := libSwDrmPlugin

LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4

include $(BUILD_PACKAGE)

include $(call all-makefiles-under, $(LOCAL_PATH))

