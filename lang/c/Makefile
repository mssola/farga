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
# Compile options. You can use CROSS_COMPILE just like on the Linux Kernel.

CC       = $(CROSS_COMPILE)gcc
LD       = $(CROSS_COMPILE)ld
CCFLAGS  = -Werror -Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual -Winit-self \
           -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
           -Wswitch-default -Wundef -Wunreachable-code -Wmissing-noreturn \
           -D_FORTIFY_SOURCE=3 -fstrict-flex-arrays=3 -fsanitize=bounds-strict
	       # NOTE: last line: special flags for the examples :)
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
