NAME = hello

CROSS_AS 	?= riscv64-suse-linux-as
CROSS_LD 	?= riscv64-suse-linux-ld
CROSS_DUMP	?= riscv64-suse-linux-objdump
ASFLAGS  	 = -march=rv64g -mabi=lp64

EMU ?= qemu-system-riscv64

##
# Targets

.PHONY: run
run: clean build
	$(CROSS_DUMP) -D $(NAME)
	$(EMU) -machine virt -bios hello

.PHONY: build
build:
	$(CROSS_AS) $(ASFLAGS) -o $(NAME).o -c $(NAME).s
	$(CROSS_LD) -T config.ld $(NAME).o -o $(NAME)

.PHONY: clean
clean:
	@rm -f $(NAME).o $(NAME)
