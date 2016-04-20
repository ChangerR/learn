#! /bin/bash

export NDK_SYSROOT=$ANDROID_NDK/platforms/android-16/arch-arm
export NDK_CROSS_PREFIX=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-
INSTALL_PATH=$(pwd)/prebuild/ffmpeg/armeabi-v7a

pushd libx264
./configure --enable-pic --host=arm-linux --cross-prefix=$NDK_CROSS_PREFIX --sysroot=$NDK_SYSROOT --prefix=$INSTALL_PATH --extra-cflags='-march=armv7-a -mtune=cortex-a8 -mfpu=neon' --enable-static 
make STRIP= -j4
make install
popd

./configure --extra-libs=-ldl --prefix=$INSTALL_PATH \
--cross-prefix=$NDK_CROSS_PREFIX --sysroot=$NDK_SYSROOT --arch=armv7-a --enable-neon --enable-armv6 \
--enable-cross-compile --target-os=linux --disable-libfreetype --enable-libx264 \
--enable-gpl --enable-nonfree --enable-version3 --disable-debug --disable-shared --enable-pic --enable-asm \
--disable-ffmpeg --disable-ffplay --disable-ffprobe --disable-ffserver --enable-static \
--extra-cflags="-mtune=cortex-a8 -fPIC -DANDROID -I ${INSTALL_PATH}/include" \
--extra-ldflags="-L ${INSTALL_PATH}/lib"
make -j4
make install

echo "Build Success!!!!!!!!!!"

exit 0
