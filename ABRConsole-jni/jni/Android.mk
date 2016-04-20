LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
$(call import-add-path,$(LOCAL_PATH)/../prebuild)

LOCAL_MODULE := hikhelper_shared

LOCAL_MODULE_FILENAME := libhikhelper

LOCAL_SRC_FILES := HikNetHelper.cpp HikHelperJni.cpp

LOCAL_LDLIBS := -llog

LOCAL_C_INCLUDES :=

LOCAL_CFLAGS :=

LOCAL_SHARED_LIBRARIES := hcnetsdk-prebuilt
LOCAL_SHARED_LIBRARIES += HCCore-prebuilt
LOCAL_SHARED_LIBRARIES += HCPreview-prebuilt
LOCAL_SHARED_LIBRARIES += HCCoreDevCfg-prebuilt

include $(BUILD_SHARED_LIBRARY)

$(call import-module,HikVision)
