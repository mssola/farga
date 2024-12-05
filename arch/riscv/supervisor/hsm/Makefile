V =
ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif

CC    = $(CROSS_COMPILE)gcc$(CC_SUFFIX)
LD    = $(CROSS_COMPILE)ld
QEMU ?= qemu-system-riscv64

SRC    = $(filter-out hsm.ld.S, $(wildcard *.S *.c))
OBJ    = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SRC)))
LINKER = hsm.ld
KERNEL = hsm

ISA      ?= rv64imafdc_zicntr_zicsr_zifencei_zihpm_zca_zcd_zba_zbb
ASFLAGS   = -march=$(ISA) -mabi=lp64d -mcmodel=medany -fno-PIE -g
CCFLAGS   = $(ASFLAGS) -std=gnu17 -nostdinc -nostdlib -O0
WARNINGS  = -Werror -Wpedantic -Wall -Wextra -Wcast-align -Wcast-qual -Winit-self \
            -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion \
            -Wswitch-default -Wundef -Wunreachable-code
CCFLAGS  += $(WARNINGS)
LDFLAGS   = -static -melf64lriscv -z noexecstack -T $(LINKER)
QEMU_FLAGS += -nographic

CPUS ?= 4

.PHONY: hsm
$(KERNEL): config $(OBJ)
	$(Q) $(CC) -E $(LINKER).S -Iinclude/ -o $(LINKER)

	$(E) "	LD	" $@
	$(Q) $(LD) $(LDFLAGS) $(OBJ) -o $(KERNEL)

.PHONY: config
config:
	$(Q) rm -f config.h
	$(Q) echo "#ifndef __CONFIG_H__" >> config.h
	$(Q) echo "#define __CONFIG_H__" >> config.h
	$(Q) echo -e "\n#define NR_CPUS $(CPUS)\n" >> config.h
	$(Q) echo "#if (NR_CPUS < 2 || NR_CPUS > 9)" >> config.h
	$(Q) echo "	#error \"Use a value of 'CPUS' > 1 and < 10\"" >> config.h
	$(Q) echo -e "#endif // #if (NR_CPUS < 2 || NR_CPUS > 9)\n" >> config.h
	$(Q) echo "#endif // __CONFIG_H__" >> config.h

.c.o:
	$(E) "	CC	" $(basename $@)
	$(Q) $(CC) $(CCFLAGS) -c $< -o $@

.S.o:
	$(E) "	CC	" $(basename $@)
	$(Q) $(CC) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(Q) rm -f *.o $(KERNEL) $(LINKER) config.h

.PHONY: qemu
qemu: clean $(KERNEL)
	$(Q) $(QEMU) $(QEMU_FLAGS) -smp $(CPUS) -machine virt -kernel $(KERNEL)
