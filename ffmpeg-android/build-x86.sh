#! /bin/bash

export NDK_SYSROOT=$ANDROID_NDK/platforms/android-16/arch-x86
export NDK_CROSS_PREFIX=$ANDROID_NDK/toolchains/x86-4.9/prebuilt/linux-x86_64/bin/i686-linux-android-
INSTALL_PATH=$(pwd)/prebuild/ffmpeg/x86

pushd libx264
./configure --enable-pic --host=i686-linux --cross-prefix=$NDK_CROSS_PREFIX --sysroot=$NDK_SYSROOT --prefix=$INSTALL_PATH --extra-cflags='-march=i686 ' --enable-static 
make STRIP= -j4
make install
popd

./configure --extra-libs=-ldl --prefix=$INSTALL_PATH \
--cross-prefix=$NDK_CROSS_PREFIX --sysroot=$NDK_SYSROOT --arch=x86 --cpu=i686 \
--enable-cross-compile --target-os=linux --disable-libfreetype --enable-libx264 \
--enable-gpl --enable-nonfree --enable-version3 --disable-debug --disable-shared --enable-pic --enable-asm \
--disable-ffmpeg --disable-ffplay --disable-ffprobe --disable-ffserver --enable-static \
--extra-cflags="-fPIC -DANDROID -I ${INSTALL_PATH}/include" \
--extra-ldflags="-L ${INSTALL_PATH}/lib"
make -j4
make install

echo "Build Success!!!!!!!!!!"

exit 0
