LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := fCap 
LOCAL_SRC_FILES := main.cpp \
                   CCapTask.cpp \
                   CNullTask.cpp \
                   CRszUtil.cpp \
                   CZcTask.cpp
LOCAL_LDFLAGS:=-fPIE -pie
LOCAL_SHARED_LIBRARIES := libACE
LOCAL_LDLIBS += -lz
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers \ /home/skwak/cct/fCap
LOCAL_CFLAGS := -DACE_NTRACE=1

include $(BUILD_EXECUTABLE)
