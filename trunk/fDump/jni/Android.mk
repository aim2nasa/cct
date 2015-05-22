LOCAL_PATH := $(call my-dir)

#Library
include $(CLEAR_VARS)
LOCAL_MODULE := libACE
LOCAL_SRC_FILES := /home/skwak/ACE_wrappers/lib/libACE.so
include $(PREBUILT_SHARED_LIBRARY)

#Executable
include $(CLEAR_VARS)

LOCAL_MODULE := fDump
LOCAL_SRC_FILES := client.cpp \
		   CRestoreTask.cpp \
		   utility.cpp \
LOCAL_LDFLAGS:=-fPIE -pie
LOCAL_SHARED_LIBRARIES := libACE
LOCAL_LDLIBS += -lz
LOCAL_C_INCLUDES += /home/skwak/ACE_wrappers \
		    /home/skwak/cct/fDump/jni \
		    /home/skwak/android-ndk-r8e/platforms/android-14/arch-arm/usr/include

include $(BUILD_EXECUTABLE)
