LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
$(call import-add-path,$(LOCAL_PATH)/../prebuild)

LOCAL_MODULE := rovcpp_shared

LOCAL_MODULE_FILENAME := librovcpp

LOCAL_SRC_FILES := MpegtsParser.cpp

LOCAL_LDLIBS := -llog 
					   
LOCAL_C_INCLUDES := 

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS
					
					
LOCAL_STATIC_LIBRARIES = postproc-prebuilt
LOCAL_STATIC_LIBRARIES += avformat-prebuilt
LOCAL_STATIC_LIBRARIES += avcodec-prebuilt 
LOCAL_STATIC_LIBRARIES += avdevice-prebuilt
LOCAL_STATIC_LIBRARIES += avfilter-prebuilt						
LOCAL_STATIC_LIBRARIES += avutil-prebuilt
LOCAL_STATIC_LIBRARIES += swresample-prebuilt 
LOCAL_STATIC_LIBRARIES += swscale-prebuilt
LOCAL_STATIC_LIBRARIES += x264-prebuilt


include $(BUILD_SHARED_LIBRARY)

$(call import-module,ffmpeg)
