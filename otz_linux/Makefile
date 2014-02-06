# Distribution example : makefile
#
# Copyright (C) ARM Limited, 2010. All rights reserved.
#
# This makefile is intended for use with GNU make

#  Building on Linux
export DIST_DIR:=$(CURDIR)

export MULTIPLE_GUESTS_SUPPORT:=n

#export FAST_MODEL_CONFIG:=EB_V6_V7
export FAST_MODEL_CONFIG:=VE_V7
LINUX_DIR:=$(DIST_DIR)/linux-otz
LINUX_EXTRACT_DIR:=$(DIST_DIR)/linux-2.6.38.7
LINUX_TAR_FILE:=linux-2.6.38.7.tar.bz2

LINUX_2_DIR:=$(DIST_DIR)/linux-otz-2

ifeq ($(FAST_MODEL_CONFIG), EB_V6_V7)
LINUX_CONFIG_FILE:=fastmodel_eb_defconfig
else
LINUX_CONFIG_FILE:=fastmodel_ve_defconfig
endif

export KERNEL_IM:= $(LINUX_DIR)/arch/arm/boot/Image

export OTZONE_ASYNC_NOTIFY_SUPPORT:= y
export OTZONE_AMP_SUPPORT:= y


ifeq ($(OTZONE_ASYNC_NOTIFY_SUPPORT), y)
export EXTRA_CFLAGS += -DOTZONE_ASYNC_NOTIFY_SUPPORT=y
endif

ifeq ($(OTZONE_AMP_SUPPORT), y)
export EXTRA_CFLAGS1 += -DOTZONE_AMP_SUPPORT=y
endif

ifeq ($(MULTIPLE_GUESTS_SUPPORT),y)
export EXTRA_CFLAGS2 += -DMULTIPLE_GUESTS_SUPPORT=y
endif

include boot/Makefile.boot

ifeq ($(MULTIPLE_GUESTS_SUPPORT),y)
include boot_guest2/Makefile.boot
endif

#all: patch_linux linux_build target_image
ifeq ($(MULTIPLE_GUESTS_SUPPORT),y)
all: patch_linux patch_linux_2 linux_build linux_2_build target_image target_image_2
else
all: patch_linux linux_build target_image
endif

ifeq ($(MULTIPLE_GUESTS_SUPPORT),y)
clean : clean_linux_1 clean_linux_2
else
clean : clean_linux_1
endif

linux_build:
	$(MAKE) ARCH=arm -C $(LINUX_DIR) $(LINUX_CONFIG_FILE) 
#	$(MAKE) ARCH=arm -C $(LINUX_DIR) silentoldconfig
	$(MAKE) EXTRA_CFLAGS=$(EXTRA_CFLAGS) EXTRA_CFLAGS+=$(EXTRA_CFLAGS1) EXTRA_CFLAGS+=$(EXTRA_CFLAGS2) -j8 ARCH=arm -C $(LINUX_DIR) uImage

linux_2_build:
	$(MAKE) ARCH=arm -C $(LINUX_2_DIR) $(LINUX_CONFIG_FILE) 
#	$(MAKE) ARCH=arm -C $(LINUX_2_DIR) silentoldconfig
	$(MAKE) EXTRA_CFLAGS=$(EXTRA_CFLAGS) EXTRA_CFLAGS+=$(EXTRA_CFLAGS1) -j8 ARCH=arm -C $(LINUX_2_DIR) uImage

target_image: 
	$(MAKE) -C $(BOOT_DIR) clean
	$(MAKE) -C $(BOOT_DIR)

target_image_2: 
	$(MAKE) -C $(BOOT_2_DIR) clean
	$(MAKE) -C $(BOOT_2_DIR)

clean_linux_1: 
	$(MAKE) ARCH=arm -C $(LINUX_DIR) clean
	$(MAKE) -C $(BOOT_DIR) clean

clean_linux_2: 
	$(MAKE) ARCH=arm -C $(LINUX_2_DIR) clean
	$(MAKE) -C $(BOOT_2_DIR) clean

patch_linux:
	if [ ! -d ${LINUX_DIR} ]; then \
	tar -jxf $(LINUX_TAR_FILE) ; mv $(LINUX_EXTRACT_DIR) $(LINUX_DIR); \
    cd $(LINUX_DIR); patch -p1 < ../patches/ael.diff;\
    cd $(LINUX_DIR); patch -p1 < ../patches/otz_linux_async.patch;\
    cd $(LINUX_DIR); patch -p1 < ../patches/otz_hyp_linux1.patch;\
	cd $(DIST_DIR); cp -f $(FAST_MODEL_CONFIG) $(LINUX_DIR)/arch/arm/configs/$(LINUX_CONFIG_FILE); \
#	cd $(DIST_DIR); cp -f $(FAST_MODEL_CONFIG) $(LINUX_DIR)/.config; \
	fi;

patch_linux_2:
	if [ ! -d ${LINUX_2_DIR} ]; then \
	tar -jxf $(LINUX_TAR_FILE) ; mv $(LINUX_EXTRACT_DIR) $(LINUX_2_DIR); \
    cd $(LINUX_2_DIR); patch -p1 < ../patches/ael.diff;\
    cd $(LINUX_2_DIR); patch -p1 < ../patches/otz_linux_async.patch;\
    cd $(LINUX_2_DIR); patch -p1 < ../patches/otz_hyp_linux2.patch;\
	cd $(DIST_DIR); cp -f $(FAST_MODEL_CONFIG) $(LINUX_2_DIR)/arch/arm/configs/$(LINUX_CONFIG_FILE); \
#	cd $(DIST_DIR); cp -f $(FAST_MODEL_CONFIG) $(LINUX_2_DIR)/.config; \
	fi;
