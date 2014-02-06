# master make file for Trustzone sdk kit

#Set this variable to y if you want to enable experimental support for
#openssl. Note that if you set this to y, the variable CONFIG_CRYPTO in the
#file config.package, found in $(PACKAGE_DIR), *MUST* be set to y. If not,
#expect compilation failures. Note that a separate excutable otzone-crypto.elf
#would be generated, which can then be used for verification purposes.
#Further, a newlib enabled toolchain is required for compiling and using openssl.
#the environment variable CROSS_COMPILE_NEWLIB must be defined appropriately.
#For example in bash -- export CROSS_COMPILE_NEWLIB=arm-none-eabi-
export ENABLE_LIBCRYPT:= n

ifeq ($(ENABLE_LIBCRYPT), y)
ifndef CROSS_COMPILE_NEWLIB
$(error Experimental support for openssl is enabled. CROSS_COMPILE_NEWLIB \
		must be defined and point to a toolchain with \
		newlib support. For example, in bash do --\
		export CROSS_COMPILE_NEWLIB=arm-none-eabi-)
endif
export CRYPTO_BUILD:= n
endif

export SVISOR_BUILD:= n
export OTZONE_BUILD:= y
export CONFIG_MULTI_GUESTS_SUPPORT:=n
export CONFIG_ANDROID_GUEST:=n
export NON_SECURE_BOOTWRAPPER_SUPPORT:=y
export CONFIG_NEON_SUPPORT :=n

export SDK_PATH:= $(CURDIR)
export PACKAGE_DIR:= $(SDK_PATH)/../package
export PACKAGE_PATCHES_DIR:=$(PACKAGE_DIR)/patches
export PACKAGE_STORAGE_DIR:=$(PACKAGE_DIR)/storage
export PACKAGE_BUILD_DIR:=$(PACKAGE_DIR)/build
export CONFIG_PACKAGE_FILE:=$(PACKAGE_DIR)/config.package
export ANDROID_PATH:= /home/pub/ov_android/android

ifeq ($(CONFIG_ANDROID_GUEST),y)
export KDIR := $(ANDROID_PATH)/kernel
else	
export KDIR := $(SDK_PATH)/../otz_linux/linux-otz
endif
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y)
export KDIR2 := $(SDK_PATH)/../otz_linux/linux-otz-2
endif

ifeq ($(CONFIG_ANDROID_GUEST),y)
export KERNEL_BUILT_DIR := $(ANDROID_PATH)/out/target/product/vexpress_rtsm/obj/kernel
else	
export KERNEL_BUILT_DIR := $(SDK_PATH)/../otz_linux/linux-otz
endif
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y)
export KERNEL_BUILT_DIR_2 := $(SDK_PATH)/../otz_linux/linux-otz-2
endif

ifeq ($(CONFIG_ANDROID_GUEST),y)
NS_IMAGE := $(ANDROID_PATH)/out/target/product/vexpress_rtsm/boot/linux-system-A15.axf
else	
NS_IMAGE := $(SDK_PATH)/../otz_linux/boot/RTSM/normal.elf
endif
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y)
NS_IMAGE_2 := $(SDK_PATH)/../otz_linux/boot_guest2/RTSM/normal.elf
INITRD_IMAGE := $(SDK_PATH)/../otz_linux/linux2_initrd.bin.gz
endif
ROOT_FILE_SYSTEM_IMAGE := $(SDK_PATH)/../otz_linux/armv5t_min_EB_V6_V7.image 

-include $(CONFIG_PACKAGE_FILE)

export BOARD:= VE
export ARM_CPU:= CORTEX_A15
export ARM_ARCH:= ARMV7

ifeq ($(ARM_CPU), CORTEX_A15)
ifneq ($(CONFIG_ANDROID_GUEST), y)
export OTZONE_ASYNC_NOTIFY_SUPPORT:= y
endif
endif


.PHONY: all

ifeq ($(CONFIG_ANDROID_GUEST),y)
all:	
else
ifneq ($(BOARD), zynq7)
all:	$(ROOT_FILE_SYSTEM_IMAGE)
else
all:
endif
endif
ifeq ($(NON_SECURE_BOOTWRAPPER_SUPPORT),y)
	cp -f $(NS_IMAGE) $(SDK_PATH)/bin/normal.elf;
else
	cp -f $(NS_IMAGE) $(SDK_PATH)/bin/normal.bin;
endif
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y)

ifeq ($(NON_SECURE_BOOTWRAPPER_SUPPORT),y)
	cp -f $(NS_IMAGE_2) $(SDK_PATH)/bin/normal_2.elf;
else
	cp -f $(NS_IMAGE_2) $(SDK_PATH)/bin/normal_2.bin;
endif
ifneq ($(BOARD), zynq7)
	cp -f $(INITRD_IMAGE) $(SDK_PATH)/otzone/linux2_initrd.bin.gz;
endif
endif

ifneq ($(CONFIG_ANDROID_GUEST),y)
	$(MAKE) -C $(SDK_PATH)/otz_api
	$(MAKE) -C $(SDK_PATH)/ns_client_apps
endif
	$(MAKE) -C $(SDK_PATH)/otz_driver
	$(MAKE) -C $(SDK_PATH)/../package

	cp -f bin/mmc_fs otzone/mmc_fs
	
ifneq ($(BOARD), zynq7)
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y)
ifneq ($(CONFIG_ANDROID_GUEST),y)
	gunzip $(SDK_PATH)/otzone/linux2_initrd.bin.gz;

	if [ -f ${SDK_PATH}/bin/otzapp.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(SDK_PATH)/otzone/linux2_initrd.bin /tmp/mnt; \
	mkdir -p /tmp/mnt/bin; \
	sudo cp ${SDK_PATH}/bin/otzapp.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otzapp.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_tee_app.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(SDK_PATH)/otzone/linux2_initrd.bin /tmp/mnt; \
	mkdir -p /tmp/mnt/bin; \
	sudo cp ${SDK_PATH}/bin/otz_tee_app.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_tee_app.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_boot_ack.elf ]; then \
        mkdir -p /tmp/mnt; \
        sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
        sudo cp ${SDK_PATH}/bin/otz_boot_ack.elf /tmp/mnt/bin; \
        sudo chmod 755 /tmp/mnt/bin/otz_boot_ack.elf;\
        sleep 1;\
        sudo umount /tmp/mnt; \
        fi

	if [ -f ${SDK_PATH}/bin/otz_virtual_keyboard.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_virtual_keyboard.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_virtual_keyboard.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_neon_app.elf ]; then \
        mkdir -p /tmp/mnt; \
        sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
        sudo cp ${SDK_PATH}/bin/otz_neon_app.elf /tmp/mnt/bin; \
        sudo chmod 755 /tmp/mnt/bin/otz_neon_app.elf;\
        sleep 1;\
        sudo umount /tmp/mnt; \
        fi

	if [ -f ${SDK_PATH}/bin/otz_client_2.ko ]; then \
	mkdir -p /tmp/mnt ;\
	sudo mount -o loop $(SDK_PATH)/otzone/linux2_initrd.bin /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_client_2.ko /tmp/mnt/; \
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/lib/libotzapi.so ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(SDK_PATH)/otzone/linux2_initrd.bin /tmp/mnt; \
	mkdir -p /tmp/mnt/usr; \
	mkdir -p /tmp/mnt/usr/lib; \
	sudo cp ${SDK_PATH}/lib/libotzapi.so /tmp/mnt/usr/lib; \
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi
	
	if [ -f ${SDK_PATH}/bin/otz_boot_ack.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_boot_ack.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_boot_ack.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_virtual_keyboard.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_virtual_keyboard.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_virtual_keyboard.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	sleep 1;
	gzip $(SDK_PATH)/otzone/linux2_initrd.bin;
endif
endif
endif

ifeq ($(CONFIG_ANDROID_GUEST),y)
ifeq ($(BOARD),VE)
	if [ -f ${SDK_PATH}/bin/otz_client.ko ]; then \
	mkdir -p /tmp/mnt ;\
	tar -xjf $(ANDROID_PATH)/out/target/product/vexpress_rtsm/system.tar.bz2 -C /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_client.ko /tmp/mnt/system/modules/; \
	cd /tmp/mnt; \
	tar -cjf $(ANDROID_PATH)/out/target/product/vexpress_rtsm/system.tar.bz2 system/; \
	sleep 1;\
	rm -rf system; \
	cd $(SDK_PATH);\
	fi
endif
endif
	

	$(MAKE) -C $(SDK_PATH)/otzone
	rm -rf otzone/mmc_fs

ifneq ($(BOARD), zynq7)
ifneq ($(CONFIG_ANDROID_GUEST),y)
	if [ -f ${SDK_PATH}/bin/otzapp.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otzapp.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otzapp.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_tee_app.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_tee_app.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_tee_app.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_boot_ack.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_boot_ack.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_boot_ack.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_virtual_keyboard.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_virtual_keyboard.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_virtual_keyboard.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_play_media.elf ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_play_media.elf /tmp/mnt/bin; \
	sudo chmod 755 /tmp/mnt/bin/otz_play_media.elf;\
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/bin/otz_neon_app.elf ]; then \
        mkdir -p /tmp/mnt; \
        sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
        sudo cp ${SDK_PATH}/bin/otz_neon_app.elf /tmp/mnt/bin; \
        sudo chmod 755 /tmp/mnt/bin/otz_neon_app.elf;\
        sleep 1;\
        sudo umount /tmp/mnt; \
        fi

	if [ -f ${SDK_PATH}/bin/otz_client.ko ]; then \
	mkdir -p /tmp/mnt ;\
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/bin/otz_client.ko /tmp/mnt/; \
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi

	if [ -f ${SDK_PATH}/lib/libotzapi.so ]; then \
	mkdir -p /tmp/mnt; \
	sudo mount -o loop $(ROOT_FILE_SYSTEM_IMAGE) /tmp/mnt; \
	sudo cp ${SDK_PATH}/lib/libotzapi.so /tmp/mnt/usr/lib; \
	sleep 1;\
	sudo umount /tmp/mnt; \
	fi
endif
endif

boot:
	$(MAKE) -C $(SDK_PATH)/otzone boot

clean:
	$(MAKE) -C $(SDK_PATH)/../package clean
	$(MAKE) -C $(SDK_PATH)/otzone clean
ifneq ($(CONFIG_ANDROID_GUEST),y)
	$(MAKE) -C $(SDK_PATH)/ns_client_apps clean
	$(MAKE) -C $(SDK_PATH)/otz_api clean
endif
	$(MAKE) -C $(SDK_PATH)/otz_driver clean

	rm -f otzone/mmc_fs
	rm -f bin/*.elf
	rm -f bin/*.bin
	rm -f bin/*.ko
