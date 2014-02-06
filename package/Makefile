# Makefile for all packages

ifndef SDK_PATH
$(error SDK_PATH must be defined before this file is included)
endif

ifndef PACKAGE_DIR
$(error PACKAGE_DIR must be defined before this file is included)
endif

ifndef PACKAGE_PATCHES_DIR
$(error PACKAGE_PATCHES_DIR must be defined before this file is included)
endif

ifndef PACKAGE_STORAGE_DIR
$(error PACKAGE_STORAGE_DIR must be defined before this file is included)
endif

ifndef PACKAGE_BUILD_DIR
$(error PACKAGE_BUILD_DIR must be defined before this file is included)
endif

#ifndef CROSS_COMPILE_NEWLIB
#export CROSS_COMPILE_NEWLIB:=arm-none-eabi-
#endif

-include $(CONFIG_PACKAGE_FILE)

export CC:=${CROSS_COMPILE_NEWLIB}gcc
export CXX:=${CROSS_COMPILE_NEWLIB}g++
export AR:= ${CROSS_COMPILE_NEWLIB}ar rcs
export RANLIB:= ${CROSS_COMPILE_NEWLIB}ranlib

export CFLAGS:= -msoft-float -mno-thumb-interwork -marm -march=armv7-a -fno-short-enums
export CXXFLAGS:= $CFLAGS

.PHONY: all

#all: otz_api ns-client-apps otzone
#all: $(ROOT_FILE_SYSTEM_IMAGE)
#	cp -f $(NS_IMAGE) $(SDK_PATH)/bin;
all:
ifeq ($(CONFIG_LIBSDL), y)
	$(MAKE) -f Makefile.libsdl
endif
ifeq ($(CONFIG_FFMPEG), y)
	$(MAKE) -f Makefile.ffmpeg
endif
ifeq ($(CONFIG_CRYPTO), y)
	$(MAKE) -f Makefile.crypto
endif
clean:
	rm -rf $(PACKAGE_BUILD_DIR)


