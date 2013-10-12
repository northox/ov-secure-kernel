SHELL := /bin/bash
ifndef SDK_PATH
$(error SDK_PATH must be defined before this file is included)
endif

cur_dir=$(SDK_PATH)/otz_api
lib_dir=$(SDK_PATH)/lib
build_dir=$(cur_dir)/build
src_dir=$(cur_dir)

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

# Name & Version
export PROJECT_NAME = libotzapi
export PROJECT_VERSION = 0.1
export DEPENDENCY_FILE=$(build_dir)/.deps

# Setup list of objects for compilation
app-object-mks=$(shell if [[ -d $(src_dir) ]]; then find $(src_dir) -iname "objects.mk" | sort -r; fi)

include $(app-object-mks) 

objs-y+=$(foreach obj,$(app-objs-y),$(build_dir)/$(obj))

# Setup list of deps files for compilation
#deps-y=$(objs-y:.o=.dep)

# Setup list of targets for compilation
#targets-y=$(build_dir)/otzapi.a
targets-y=$(build_dir)/libotzapi.so

# Setup compilation environment
cpp=$(CROSS_COMPILE)cpp
cc=$(CROSS_COMPILE)gcc
cflags= -g -Wall -nostdlib -fPIC
cflags+=-msoft-float -mno-thumb-interwork -marm  -fno-short-enums -I$(SDK_PATH)/include
cflags+=$(cppflags) $(TARGET_CCFLAGS)
as=$(CROSS_COMPILE)gcc
asflags=-g -Wall -D__ASSEMBLY__ 
asflags+= -mno-thumb-interwork -marm
asflags+=$(cppflags) $(TARGET_ASMFLAGS)
ar=$(CROSS_COMPILE)ar
arflasgs=rcs
ld=$(CROSS_COMPILE)gcc
ldflags=-g -Wall
objcopy=$(CROSS_COMPILE)objcopy
nm=$(CROSS_COMPILE)nm

final-objs-y=$(objs-y)


# Default rule "make"
.PHONY: all
all: $(targets-y)

test:
	@echo $(objs-y)

# Rules to build .S and .c files
$(build_dir)/libotzapi.so: $(final-objs-y)
	$(V)mkdir -p `dirname $@`
	$(if $(V), @echo " (ld)        $(subst $(build_dir)/,,$@)")
	$(V)$(cc) -shared -Wl,-soname,libotzapi.so  $(final-objs-y) -o $@
	$(V)cp -f $@ $(lib_dir)

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
ifeq ($(build_dir),$(cur_dir)/build)
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
