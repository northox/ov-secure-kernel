SHELL := /bin/bash
ifndef SDK_PATH
$(error SDK_PATH must be defined before this file is included)
endif

build_dir=$(CURDIR)/build
src_dir=$(CURDIR)
sdk_include_dir=$(SDK_PATH)/include
lib_dir=$(SDK_PATH)/lib
bin_dir=$(SDK_PATH)/bin

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

ifeq ($(ARM_CPU), CORTEX_A8)
TARGET_CCFLAGS= -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
TARGET_ASMFLAGS= -DCONFIG_CORTEX_A8=1 -mcpu=cortex-a8
endif

ifeq ($(ARM_CPU), CORTEX_A9)
TARGET_CCFLAGS= -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
TARGET_ASMFLAGS= -DCONFIG_CORTEX_A9=1 -mcpu=cortex-a9
endif

ifeq ($(ARM_CPU), CORTEX_A15)
TARGET_CCFLAGS= -DCONFIG_CORTEX_A15=1 -march=armv7-a
TARGET_ASMFLAGS= -DCONFIG_CORTEX_A15=1 -march=armv7-a
endif

ifeq ($(OTZONE_ASYNC_NOTIFY_SUPPORT), y)
TARGET_CCFLAGS += -DOTZONE_ASYNC_NOTIFY_SUPPORT=y
TARGET_ASMFLAGS += -DOTZONE_ASYNC_NOTIFY_SUPPORT=y
endif

# Name & Version
export PROJECT_NAME = otzapp
export PROJECT_VERSION = 0.1
export DEPENDENCY_FILE=$(build_dir)/.deps

	# Setup list of objects for compilation
app-object-mks=$(shell if [[ -d $(src_dir) ]]; then find $(src_dir) -iname "objects.mk" | sort -r; fi)

include $(app-object-mks) 

objs-y=$(foreach obj,$(app-objs-y),$(build_dir)/$(obj))

objs1-y=$(foreach obj,$(app1-objs-y),$(build_dir)/$(obj))

objs2-y=$(foreach obj,$(app2-objs-y),$(build_dir)/$(obj))

objs3-y=$(foreach obj,$(app3-objs-y),$(build_dir)/$(obj))

objs4-y=$(foreach obj,$(app4-objs-y),$(build_dir)/$(obj))

ifeq ($(CONFIG_NEON_SUPPORT), y)
objs5-y=$(foreach obj,$(app5-objs-y),$(build_dir)/$(obj))
endif


# Setup list of deps files for compilation
#deps-y=$(objs-y:.o=.dep)

# Setup list of targets for compilation
targets-y=$(build_dir)/otzapp.elf $(build_dir)/otz_tee_app.elf $(build_dir)/otz_boot_ack.elf $(build_dir)/otz_virtual_keyboard.elf $(build_dir)/otz_play_media.elf 

ifeq ($(CONFIG_NEON_SUPPORT), y)
targets-y+=$(build_dir)/otz_neon_app.elf
endif
# Setup compilation environment
cpp=$(CROSS_COMPILE)cpp
cc=$(CROSS_COMPILE)gcc
cflags= -g -Wall -nostdlib 
cflags+=-msoft-float -mno-thumb-interwork -marm -fno-short-enums -I$(sdk_include_dir) 
ifeq ($(CRYPTO_BUILD), y)
cflags+=-D_CRYPTO_BUILD_
endif
ifeq ($(CONFIG_NEON_SUPPORT), y)
cflags+=-mfloat-abi=softfp -mfpu=neon
endif
cflags+=$(cppflags) $(TARGET_CCFLAGS) 
as=$(CROSS_COMPILE)gcc
asflags=-g -Wall -D__ASSEMBLY__ 
asflags+= -mno-thumb-interwork -marm 
ifeq ($(CONFIG_NEON_SUPPORT), y)
asflags+=-mfpu=neon
endif
asflags+=$(cppflags) $(TARGET_ASMFLAGS)
ar=$(CROSS_COMPILE)ar
arflasgs=rcs
ld=$(CROSS_COMPILE)gcc
ldflags=-g -Wall -L$(lib_dir) -lotzapi
objcopy=$(CROSS_COMPILE)objcopy
nm=$(CROSS_COMPILE)nm

final-objs-y=$(objs-y)
final1-objs-y=$(objs1-y)
final2-objs-y=$(objs2-y)
final3-objs-y=$(objs3-y)
final4-objs-y=$(objs4-y)
ifeq ($(CONFIG_NEON_SUPPORT), y)
final5-objs-y=$(objs5-y)
endif

# Default rule "make"
.PHONY: all
all: $(targets-y)

test:
	@echo $(objs-y)
	
# Rules to build .S and .c files
$(build_dir)/otzapp.elf: $(final-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final-objs-y) -o $@
	$(V)cp $@ $(bin_dir)

$(build_dir)/otz_tee_app.elf: $(final1-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final1-objs-y) -o $@
	$(V)cp $@ $(bin_dir)

$(build_dir)/otz_boot_ack.elf: $(final2-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final2-objs-y) -o $@
	$(V)cp $@ $(bin_dir)

$(build_dir)/otz_virtual_keyboard.elf: $(final3-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final3-objs-y) -o $@
	$(V)cp $@ $(bin_dir)

$(build_dir)/otz_play_media.elf: $(final4-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final4-objs-y) -o $@
	$(V)cp $@ $(bin_dir)

ifeq ($(CONFIG_NEON_SUPPORT), y)
$(build_dir)/otz_neon_app.elf: $(final5-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(ld)  $(ldflags) $(final5-objs-y) -o $@
	$(V)cp $@ $(bin_dir)
endif

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
ifeq ($(build_dir),$(CURDIR)/build)
	$(if $(V), @echo " (rm)        $(build_dir)")
	$(V)rm -rf $(build_dir)
endif

.PHONY: tags
tags:
	$(V)ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
	$(V)echo "Generating tags ..."

.PHONY: cscope
cscope:
	$(V)echo "Generating cscope database ..."
	$(V)find ./ -name "*.[CHSchs]" > cscope.files
	$(V)cscope -bqk
