LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := fSvr 
LOCAL_SRC_FILES := server.cpp \
                   CStreamHandler.cpp \
                   ../../fCap/jni/CCapTask.cpp \
		   ../../fCap/jni/CRszUtil.cpp \
		   ../../fCap/jni/CZcTask.cpp
LOCAL_LDFLAGS:=-fPIE -pie
LOCAL_SHARED_LIBRARIES := libACE
LOCAL_LDLIBS += -lz
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers \
                    /home/skwak/cct/fCap/jni

include $(BUILD_EXECUTABLE)
