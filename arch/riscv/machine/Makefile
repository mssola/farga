NAME = machine

CROSS_COMPILE ?= riscv64-suse-linux-
AS             = $(CROSS_COMPILE)as
LD             = $(CROSS_COMPILE)ld
ASFLAGS        = -g -march=rv64g -mabi=lp64d
QEMU          ?= qemu-system-riscv64
QEMU_FLAGS    ?=

DEBUG =
ifneq ($(strip $(DEBUG)),)
	QEMU_FLAGS += -s -S
endif

##
# Targets

.PHONY: run
run: clean build
	@$(QEMU) $(QEMU_FLAGS) -machine virt -bios $(NAME) -nographic

.PHONY: build
build:
	@$(AS) $(ASFLAGS) -o $(NAME).o -c $(NAME).S
	@$(LD) -T config.ld $(NAME).o -o $(NAME)

.PHONY: clean
clean:
	@rm -f $(NAME).o $(NAME)

.PHONY: gdb
gdb:
	$(Q) gdb --command gdb.txt
