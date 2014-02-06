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
BOOT_2_DIR:=$(DIST_DIR)/boot_guest2/$(BOOT)/$(FAST_MODEL_CONFIG)
