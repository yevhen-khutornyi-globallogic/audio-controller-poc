# /platform/core/shared/sysroot/prime/bin/clang++ -m32 -march=armv7-a -mthumb -mfpu=vfpv3-d16 --sysroot=/platform/core/shared/sysroot/prime --target=arm-linux-gnueabihf main.cpp
CXX= /platform/core/shared/sysroot/prime/bin/clang++
CFLAGS=-m32 -march=armv7-a -mthumb -mfpu=vfpv3-d16 --sysroot=/platform/core/shared/sysroot/prime --target=arm-linux-gnueabihf
CXX_FLAGS=$(CFLAGS) -stdlib=libc++ 

audio-controller: main.cpp
	$(CXX) $(CXX_FLAGS) -o $@ $^
