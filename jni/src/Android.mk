LOCAL_PATH:= $(call my-dir)

#LOCAL_MODULE := gnustl_static
#定义工程的更目录，在工程目录发送改变时，修改此值
LOCAL_PROJECT_ROOT := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)))
$(warning "the value of LOCAL_PROJECT_ROOT is $LOCAL_PROJECT_ROOT")



##################################################
###                mediastream                 ###
##################################################

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__OS_ANDROID
LOCAL_CFLAGS += -D__CHIP_VERSION_F23
LOCAL_MODULE_TAGS := optional


LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/RtpTrans/rtplib/jrtplib/*.c*)) \
	$(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/Common/*.c*))

LOCAL_C_INCLUDES := $(LOCAL_PROJECT_ROOT) \
	$(LOCAL_PROJECT_ROOT)/RtpTrans \
    $(LOCAL_PROJECT_ROOT)/RtpTrans/rtplib/jrtplib \
	$(LOCAL_PROJECT_ROOT)/Common	\
	$(LOCAL_PROJECT_ROOT)/include	\
	external/stlport/stlport bionic
	
LOCAL_SHARED_LIBRARIES := \
	
LOCAL_SHARED_LIBRARIES += libstlport 
LOCAL_LDLIBS := -llog
LOCAL_MODULE:= libMediaStream

include $(BUILD_STATIC_LIBRARY)


