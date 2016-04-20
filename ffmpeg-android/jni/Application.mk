NDK_TOOLCHAIN_VERSION = 4.9
APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic -lc
APP_PLATFORM := android-16
APP_ABI := armeabi-v7a x86
APP_STL := gnustl_static

ifeq ($(NDK_DEBUG),1)
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
