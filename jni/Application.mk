LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_ABI := 	armeabi-v7a #arm64-v8a
APP_PLATFORM := android-30 #android-28
APP_CPPFLAGS += -std=c++17
APP_STL := c++_static
APP_OPTIM := release
APP_CFLAGS := -Oz
#APP_CFLAGS := -g0
