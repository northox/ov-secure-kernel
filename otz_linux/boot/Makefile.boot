# Kernel module example : makefile
#  
# Copyright (C) ARM Limited, 2010. All rights reserved.
#
# This makefile is intended for use with GNU make
# It is designed to be included in other Makefiles

ifndef DIST_DIR
$(error DIST_DIR must be defined before this file is included)
endif

CROSS_COMPILE = arm-none-linux-gnueabi-

BOOT=RTSM
#KERNEL=linux-2.6.38-ael-11.06-patched
KERNEL=
CONFIG=$(FAST_MODEL_CONFIG)

BOOT_DIR:=$(DIST_DIR)/boot/$(BOOT)/$(FAST_MODEL_CONFIG)
#KERNEL_TOP_DIR:=$(DIST_DIR)/kernel/$(KERNEL)
KERNEL_TOP_DIR:=$(DIST_DIR)/$(KERNEL)
KERNEL_SOURCE_DIR:=$(KERNEL_TOP_DIR)/source
KERNEL_BUILT_DIR:=$(KERNEL_TOP_DIR)/built
