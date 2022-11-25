
:: x86
:: https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z/download
:: https://vulkan.lunarg.com/sdk/home#windows

mkdir -p build-x86
pushd build-x86
cmake -G "MinGW Makefiles" ..
mingw32-make -j8
:: make install
popd
