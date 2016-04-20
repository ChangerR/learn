NDK_TOOLCHAIN_VERSION = 4.9
APP_CPPFLAGS :=
APP_LDFLAGS := -latomic -lc
APP_PLATFORM := android-9
APP_ABI := armeabi
APP_STL := gnustl_shared

ifeq ($(NDK_DEBUG),1)
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
