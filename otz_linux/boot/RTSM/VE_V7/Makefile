# Build an ELF linux image

BOOTLOADER	= boot.S
#KERNEL		= ../../../kernel/linux-2.6.38-ael-11.06-patched/built/VE_V7/arch/arm/boot/Image

IMAGE       = ../normal.elf
LD_SCRIPT	= model.lds

CROSS_COMPILE	= arm-none-linux-gnueabi-

AS		= $(CROSS_COMPILE)as -g
LD		= $(CROSS_COMPILE)ld -g

all: $(IMAGE)

clean:
	rm -f $(IMAGE) boot.o

$(IMAGE): boot.o $(LD_SCRIPT) $(KERNEL)
	$(LD) -o $@ --script=$(LD_SCRIPT)

boot.o: $(BOOTLOADER)
	$(AS) -o $@ $<
