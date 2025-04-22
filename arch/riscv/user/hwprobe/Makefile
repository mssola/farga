##
# By default everything is silent. If you want to change this behavior, simply
# assign V=1 when calling make.

V =
ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif

##
# Compile options. You can use CROSS_COMPILE just like on the Linux Kernel. You
# can also determine the ISA to be used, which defaults to my VisionFive2 board.
# I did not go too much into the rabbit hole of platform-specific flags because
# everything is in assembly anyways and everything is statically linked. Hence
# no `-mcpu`, no `-mtune`, no funny business.

CC       = $(CROSS_COMPILE)gcc
LD       = $(CROSS_COMPILE)ld
ISA     ?= rv64imafdc_zicntr_zicsr_zifencei_zihpm_zca_zcd_zba_zbb
WARNINGS = -Werror -Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual -Winit-self \
           -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
           -Wswitch-default -Wundef -Wunreachable-code -Wmissing-noreturn
CCFLAGS  = $(WARNINGS) -march=$(ISA) -static -I$(LINUX_HEADERS)
LDFLAGS  =


# You can pass an optional `DEBUG` variable to manipulate the build type.
DEBUG =
ifeq ($(strip $(DEBUG)),)
	CCFLAGS += -O3
else
	CCFLAGS += -g
endif

##
# Paths.

SRC  = $(wildcard *.c)
EXES = $(SRC:.c=)

##
# Targets

.PHONY: all
all: clean $(EXES)

.PHONY: build
build: $(EXES)

.c:
	$(E) "	CC	" $(*F)
	$(Q) $(CC) $(CCFLAGS) $< -o $@

.PHONY: clean
clean:
	$(Q) rm -f $(EXES)
