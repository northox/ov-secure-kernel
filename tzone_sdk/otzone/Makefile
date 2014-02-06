ifndef SDK_PATH
$(error SDK_PATH must be defined before this file is included)
endif

export $BOARD
export $ARM_CPU
export $ARM_ARCH
export $ENABLE_LIBCRYPT

.PHONY: all
all: 
	if [ ! -d src/package ]; then \
	mkdir -p src/package/src; \
	cp -R $(SDK_PATH)/../package/src src/package/; \
	fi

	$(MAKE) -f Makefile.in modules
	$(MAKE) -f Makefile.in clean
	
	$(MAKE) -f Makefile.in
	$(MAKE) -f Makefile.boot
ifeq ($(ENABLE_LIBCRYPT), y)	

	$(MAKE) -f Makefile.crypto modules
	$(MAKE) -f Makefile.crypto clean
	$(MAKE) -f Makefile.crypto
endif
#	$(MAKE) -f Makefile.libc

	rm -r -f src/package

boot:
	$(MAKE) -f Makefile.boot

.PHONY: clean
clean:
	$(MAKE) -f Makefile.in clean
	$(MAKE) -f Makefile.boot clean
ifeq ($(ENABLE_LIBCRYPT), y)	
	$(MAKE) -f Makefile.crypto clean
endif
#	$(MAKE) -f Makefile.libc clean
	rm -r -f src/package


