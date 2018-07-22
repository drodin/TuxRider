LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tuxrider

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/src \
    $(LOCAL_PATH)/android \
    $(LOCAL_PATH)/../tcl8.6a3/generic \

LOCAL_CFLAGS := \
    -D__APPLE__ \
    -DTUXRACER_NO_ASSERT \
    -DLIB_CLASS=\"com/drodin/tuxrider/NativeLib\" \
    -DDATA_DIR=\"\\/sdcard\\/com.drodin.tuxrider\\/\" \
	--include $(LOCAL_PATH)/android/android.h \

SDL_SRCS := \
	src/*.c \
    src/*.cpp \
    android/*.c \

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(foreach F, $(SDL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

LOCAL_STATIC_LIBRARIES := tcl

LOCAL_LDLIBS := -llog -ldl -lc -lm -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)
