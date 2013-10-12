SHELL := /bin/bash
cur_dir=$(SDK_PATH)/otzone
build_dir=$(cur_dir)/build_boot
src_dir=$(cur_dir)/src
package_dir=$(src_dir)/package

bin_dir=$(cur_dir)/../bin

# Check if verbosity is ON for build process
VERBOSE_DEFAULT    := 0
CMD_PREFIX_DEFAULT := @
ifdef VERBOSE
	ifeq ("$(origin VERBOSE)", "command line")
		VB := $(VERBOSE)
	else
		VB := $(VERBOSE_DEFAULT)
	endif
else
	VB := $(VERBOSE_DEFAULT)
endif
ifeq ($(VB), 1)
	V :=
else
	V := $(CMD_PREFIX_DEFAULT)
endif

# Name & Version
export PROJECT_NAME = sw_bl
export PROJECT_VERSION = 0.1
export DEPENDENCY_FILE=$(build_dir)/.deps

ifeq ($(BOARD), VE)
export CONFIG_FILE= $(cur_dir)/config/config.ve
export BOARD_DIR=ve
else ifeq ($(BOARD), s5pv310)
export CONFIG_FILE= $(cur_dir)/config/config.s5pv310
export BOARD_DIR=exynos4
else ifeq ($(BOARD), mvv4412)
export CONFIG_FILE= $(cur_dir)/config/config.mvv4412
export BOARD_DIR=exynos4
else
export CONFIG_FILE= $(cur_dir)/config/config.eb
export BOARD_DIR=eb
endif

ifeq ($(ARM_ARCH), ARMV7)
export ARCH_DIR=armv7
endif

# Include configuration file if present
-include $(CONFIG_FILE)

ifeq ($(ARM_CPU), CORTEX_A8)
TARGET_CCFLAGS += -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
TARGET_ASMFLAGS += -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
export PLATFORM_DIR=Cortex-A8
endif

ifeq ($(ARM_CPU), CORTEX_A9)
TARGET_CCFLAGS += -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
TARGET_ASMFLAGS += -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
export PLATFORM_DIR=Cortex-A9
endif

ifeq ($(ARM_CPU), CORTEX_A15)
TARGET_CCFLAGS += -DCONFIG_CORTEX_A15=1 -march=armv7-a
TARGET_ASMFLAGS += -DCONFIG_CORTEX_A15=1 -march=armv7-a
export PLATFORM_DIR=Cortex-A15
endif

# Setup path of directories
export lib_dir=$(src_dir)/lib
export cpu_arch_dir=$(src_dir)/arch/arm/$(ARCH_DIR)
export cpu_platform_dir=$(src_dir)/arch/arm/$(ARCH_DIR)/$(PLATFORM_DIR)
export package_src_dir=$(package_dir)/src
export drivers_common_dir=$(src_dir)/drivers/common
export drivers_dir=$(src_dir)/drivers/$(BOARD_DIR)

LD_SCRIPT	= $(drivers_dir)/linker_boot.ld

# Setup list of objects for compilation
lib-object-mks=$(shell if [[ -d $(lib_dir) ]]; then find $(lib_dir) -iname "objects.mk" | sort -r; fi)
cpu-object-mks=$(shell if [ -d $(cpu_arch_dir) ]; then find $(cpu_arch_dir) -iname "objects.mk" | sort -r; fi)
cpu-platform-mks=$(shell if [ -d $(cpu_platform_dir) ]; then find $(cpu_platform_dir) -iname "platform_objects.mk" | sort -r; fi)
package-object-mks=$(shell if [ -d $(package_src_dir) ]; then find $(package_src_dir) -iname "objects.mk" | sort -r; fi)
drivers-common-object-mks=$(shell if [ -d $(drivers_common_dir) ]; then find $(drivers_common_dir) -iname "objects.mk" | sort -r; fi)
drivers-object-mks=$(shell if [ -d $(drivers_dir) ]; then find $(drivers_dir) -iname "objects.mk" | sort -r; fi)

include $(lib-object-mks) 
include $(package-object-mks) 
include $(drivers-common-object-mks) 
include $(drivers-object-mks) 
include $(cpu-object-mks) 
include $(cpu-platform-mks) 

objs-y=$(foreach obj,$(cpu-boot-objs-y),$(build_dir)/arch/arm/$(ARCH_DIR)/$(obj))
objs-y+=$(foreach obj,$(package-boot-objs-y),$(build_dir)/package/src/$(obj))
objs-y+=$(foreach obj,$(drivers-common-boot-objs-y),$(build_dir)/drivers/common/$(obj))
objs-y+=$(foreach obj,$(drivers-boot-objs-y),$(build_dir)/drivers/$(BOARD_DIR)/$(obj))
objs-y+=$(foreach obj,$(lib-boot-objs-y),$(build_dir)/lib/$(obj))

# Setup list of deps files for compilation
deps-y=$(objs-y:.o=.dep)

# Setup list of targets for compilation
targets-y=$(build_dir)/sw_bl.elf
targets-y+=$(build_dir)/sw_bl.bin

cpp=$(CROSS_COMPILE)cpp
cppflags=-I$(SDK_PATH)/include
cppflags+=-I$(src_dir)/lib/include
cppflags+=-I$(src_dir)/arch/arm/$(ARCH_DIR)/include
cppflags+=-I$(package_src_dir)/include
cppflags+=-I$(src_dir)/drivers/common/include
cppflags+=-I$(src_dir)/drivers/$(BOARD_DIR)/include
cppflags+=-I$(src_dir)/arch/arm/$(ARCH_DIR)/$(PLATFORM_DIR)/include
cppflags+=-I$(src_dir)/core/include

cc=$(CROSS_COMPILE)gcc
cflags=-g -Wall  -DCONFIG_SW_BL -nostdlib 
cflags+=-msoft-float -mno-thumb-interwork -marm -fno-short-enums 
cflags+=$(cppflags) $(TARGET_CCFLAGS)
as=$(CROSS_COMPILE)gcc
asflags=-g -Wall -D__ASSEMBLY__ -nostdlib

asflags+= -mno-thumb-interwork -marm -DCONFIG_SW_BL
asflags+=$(cppflags) $(TARGET_ASMFLAGS)
ar=$(CROSS_COMPILE)ar
arflasgs=rcs
ld=$(CROSS_COMPILE)gcc
ldflags=-g -Wall -nostdlib

ldflags+=-Wl,-T$(LD_SCRIPT) -nostartfiles
objcopy=$(CROSS_COMPILE)objcopy
nm=$(CROSS_COMPILE)nm

final-objs-y=$(objs-y)

# Default rule "make"

ifeq ($(CONFIG_SW_BL), y)
.PHONY: all
all: $(CONFIG_FILE) $(DEPENDENCY_FILE)  $(targets-y)
else
.PHONY: all
all: 
	@echo "secure boot loader not supported"
endif

test:
	@echo $(objs-y) $(targets-y)

-include $(DEPENDENCY_FILE)
$(DEPENDENCY_FILE): $(deps-y)
	@echo "where is dep file"
	$(V)cat $(deps-y) > $(DEPENDENCY_FILE)

# Rules to build .S and .c files
$(build_dir)/sw_bl.bin: $(build_dir)/sw_bl.elf
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (objcopy)   $(subst $(build_dir)/,,$@)")
	$(V)$(objcopy) -O binary $< $@
	$(V)cp -f $@ $(bin_dir)


$(build_dir)/sw_bl.elf: $(final-objs-y) 
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld) $(final-objs-y) $(ldflags) -o $@
	$(V)cp -f $@ $(bin_dir)


$(build_dir)/%.dep: $(src_dir)/%.S
	@echo "gen dep file"
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (as-dep)    $(subst $(build_dir)/,,$@)")
	$(V)echo -n `dirname $@`/ > $@
	$(V)$(as) $(asflags) -I`dirname $<` -MM $< >> $@

$(build_dir)/%.dep: $(src_dir)/%.c
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (cc-dep)    $(subst $(build_dir)/,,$@)")
	$(V)echo -n `dirname $@`/ > $@
	$(V)$(cc) $(cflags) -I`dirname $<` -MM $< >> $@

$(build_dir)/%.o: $(src_dir)/%.S
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (as)        $(subst $(build_dir)/,,$@)")
	$(V)$(as) $(asflags) -I`dirname $<` -c $< -o $@

$(build_dir)/%.o: $(build_dir)/%.S
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (as)        $(subst $(build_dir)/,,$@)")
	$(V)$(as) $(asflags) -I`dirname $<` -c $< -o $@

$(build_dir)/%.o: $(src_dir)/%.c
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (cc)        $(subst $(build_dir)/,,$@)")
	$(V)$(cc) $(cflags) -I`dirname $<` -c $< -o $@

$(build_dir)/%.o: $(build_dir)/%.c
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (cc)        $(subst $(build_dir)/,,$@)")
	$(V)$(cc) $(cflags) -I`dirname $<` -c $< -o $@

# Rule for "make clean"
.PHONY: clean
clean:
ifeq ($(build_dir),$(cur_dir)/build_boot)
	$(if $(V), @echo " (rm)        $(build_dir)")
	$(V)rm -rf $(build_dir)
endif
	rm -f $(offsets-file)
	rm -f $(cpu_arch_dir)/asm-offsets.s; 

.PHONY: tags
tags:
	$(V)ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
	$(V)echo "Generating tags ..."

.PHONY: cscope
cscope:
	$(V)echo "Generating cscope database ..."
	$(V)find ./ -name "*.[CHSchs]" > cscope.files
	$(V)cscope -bqk
