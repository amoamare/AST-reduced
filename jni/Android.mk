LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libbinder
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/$(APP_PLATFORM)/libbinder.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libutils
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/$(APP_PLATFORM)/libutils.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)


ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
	LOCAL_MODULE := AST64
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
	LOCAL_MODULE := AST32
endif

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/**/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/**/*.c)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

#LOCAL_LDLIBS := -L$(LOCAL_PATH)/lib -llog -g 
#LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -landroid -g 

LOCAL_SHARED_LIBRARIES := \
		libbinder \
		libutils

LOCAL_C_INCLUDES := bionic
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
$(LOCAL_PATH)/include/system/frameworks/native/include \
$(LOCAL_PATH)/include/system/core/include \
$(LOCAL_PATH)/include/system/core/include/cutils \
$(LOCAL_PATH)/Amosoft \


#LOCAL_EXPORT_C_INCLUDES:= $(LOCAL_PATH)/include
LOCAL_ALLOW_UNDEFINED_REFERENCE := true


# Enable PIE manually. Will get reset on $(CLEAR_VARS). This
# is what enabling PIE translates to behind the scenes.
#LOCAL_CFLAGS := -mllvm -sub -mllvm -fla -mllvm -bcf -mllvm -sobf -Wall -fPIE -fexceptions
LOCAL_LDFLAGS += \
	-fPIE	\
	-pie	\
	-Wl,--export-dynamic  \
	-Wl,--version-script,$(LOCAL_PATH)/include/sigchainlib/version-script.txt \
	-Wl,-rpath,/system/lib \
	-Wl,-rpath,. \
	-Wl,-rpath,/ \
	-Wl,-rpath,/apex/com.android.runtime/lib \
	-Wl,-rpath,/apex/com.android.art/lib \
	-Wl,-rpath,../../../apex/com.android.runtime/lib \
	-WL,-rpath,../../../apex/com.android.art/lib \
	-Wl,-rpath,../../../. \
	-Wl,-rpath,../../../ 
	

ifeq ($(APP_OPTIM), release)
LOCAL_CFLAGS := \
	-mllvm -fla  \
	-mllvm -sobf \
	-mllvm -bcf \
	-mllvm -bcf_loop=1 \
	-mllvm -sub	\
	-mllvm -sub_loop=1  \
	-fexceptions 
else
LOCAL_CFLAGS := \
	-fexceptions 
endif
include $(BUILD_EXECUTABLE)
