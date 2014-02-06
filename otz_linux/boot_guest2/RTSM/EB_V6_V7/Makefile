# Build an ELF linux image
ifndef KERNEL_IM
$(error KERNEL_IM must be defined before this file is included)
endif

BOOTLOADER	= boot.S
#KERNEL		= ../../../kernel/linux-2.6.38-ael-11.06-patched/built/EB_V6_V7/arch/arm/boot/Image

IMAGE       = ../normal.elf
LD_SCRIPT	= model.lds

CROSS_COMPILE	= arm-none-linux-gnueabi-

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld 

all: $(IMAGE)

clean:
	rm -f $(IMAGE) boot.o

$(IMAGE): boot.o $(LD_SCRIPT) $(KERNEL)
	$(LD) -o $@ --script=$(LD_SCRIPT)

boot.o: $(BOOTLOADER)
	echo $(KERNEL)
	$(AS) -o $@ $<
