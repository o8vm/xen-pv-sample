CC=gcc
ARCH = x86_32

LDFLAGS  += -nostdlib
CFLAGS   += -Wall -Werror -std=c99
ASFLAGS  += -D__ASSEMBLY__

ifeq ($(ARCH),x86_32)
$(info compilation for i386)
CFLAGS  += -m32 -march=i686
LDFLAGS += -m elf_i386 -T x86_32.ld
ASFLAGS += -m32
OBJS    += bootstrap.x86_32.o gettimeofday.o printf.o 
endif
ifeq ($(ARCH),x86_64)
$(info compilation for amd64)
CFLAGS  += -m64
ASFLAGS += -m64
LDFLAGS += -m elf_x86_64 -T x86_64.ld
OBJS    += bootstrap.x86_64.o
endif


.PHONY: all

all: clean testkernel

testkernel: $(OBJS) kernel.o
	@echo Compiled for ARCH=${ARCH}
	$(LD) $(LDFLAGS) $^ -o testkernel

info:
	@echo make can be run with var ARCH={x86_32,x86_64}
	@echo By default ARCH=x86_32

clean:
	rm -f *.o 
	rm -f testkernel