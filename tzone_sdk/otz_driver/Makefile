ifndef SDK_PATH
$(error SDK_PATH must be defined before this file is included)
endif

ifeq ($(ARM_CPU), CORTEX_A8)
TARGET_CCFLAGS = -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
TARGET_ASMFLAGS = -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
endif

ifeq ($(ARM_CPU), CORTEX_A9)
TARGET_CCFLAGS = -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
TARGET_ASMFLAGS = -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
endif

ifeq ($(ARM_CPU), CORTEX_A15)
TARGET_CCFLAGS = -DCONFIG_CORTEX_A15=1 -march=armv7-a
TARGET_ASMFLAGS = -DCONFIG_CORTEX_A15=1 -march=armv7-a
endif

ifeq ($(OTZONE_ASYNC_NOTIFY_SUPPORT), y)
TARGET_CCFLAGS += -DOTZONE_ASYNC_NOTIFY_SUPPORT=y
TARGET_ASMFLAGS += -DOTZONE_ASYNC_NOTIFY_SUPPORT=y
endif

PWD := $(SDK_PATH)/otz_driver

ccflags-y:= -I$(SDK_PATH)/include

EXTRA_CFLAGS=  -g -Wall -nostdlib 
EXTRA_CFLAGS+=-msoft-float -mno-thumb-interwork -marm -fno-short-enums $(TARGET_CCFLAGS)

obj-m := otz_client.o
otz_client-objs := otz_client_main.o
all: target_modules

target_modules:
	$(MAKE) ARCH=arm  -C $(KDIR) O=$(KERNEL_BUILT_DIR) M=$(PWD) modules
	cp $(PWD)/otz_client.ko $(SDK_PATH)/bin/otz_client.ko
ifeq ($(CONFIG_MULTI_GUESTS_SUPPORT),y) 
	$(MAKE) ARCH=arm  -C $(KDIR2) O=$(KERNEL_BUILT_DIR_2) M=$(PWD) modules
	cp $(PWD)/otz_client.ko $(SDK_PATH)/bin/otz_client_2.ko
endif

clean:
	$(MAKE) ARCH=arm  -C $(KDIR) M=$(PWD) clean
