LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tuxrider

LOCAL_CFLAGS := \
    -D__APPLE__ \
    -DLIB_CLASS=\"com/drodin/tuxrider/NativeLib\" \
    -DDATA_DIR=\"/sdcard/com.drodin.tuxrider/\" \
    -I$(LOCAL_PATH)/src \
    -I$(LOCAL_PATH)/android \
    -I$(LOCAL_PATH)/../tcl8.6a3/generic \
    --include $(LOCAL_PATH)/android/android.h

SDL_SRCS := \
	src/*.c \
    src/*.cpp \
    android/*.c \

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(foreach F, $(SDL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

LOCAL_STATIC_LIBRARIES := tcl

LOCAL_LDLIBS := -llog -ldl -lc -lm -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)
