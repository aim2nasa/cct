LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := fSvr 
LOCAL_SRC_FILES := server.cpp
LOCAL_LDFLAGS:=-fPIE -pie
LOCAL_SHARED_LIBRARIES := libACE
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers 

include $(BUILD_EXECUTABLE)
