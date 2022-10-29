#!/usr/bin/env bash

############################################
## android
ANDROID_NDK=/home/shared_dir/android-ndk-r21e/

##### android armv7
mkdir -p build-android-armv7
pushd build-android-armv7
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI="armeabi-v7a" -DANDROID_PLATFORM=android-21 ..
make -j4
# make install
popd

# ##### android armv7
# mkdir -p build-android-armv7
# pushd build-android-armv7
# cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI="armeabi-v7a" -DANDROID_ARM_NEON=ON -DANDROID_PLATFORM=android-24 -DNCNN_VULKAN=ON ..
# make -j4
# make install
# popd

# ##### android aarch64
# mkdir -p build-android-aarch64
# pushd build-android-aarch64
# cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI="arm64-v8a" -DANDROID_PLATFORM=android-24 -DNCNN_VULKAN=ON ..
# make -j4
# make install
# popd