CC       = $(CROSS_COMPILE)gcc
LD       = $(CROSS_COMPILE)ld
CCFLAGS  =
EXE      = basics

DEBUG =
ifneq ($(strip $(DEBUG)),)
	CCFLAGS += -g
endif

.PHONY: all
all: clean $(EXE)

.PHONY: $(EXE)
$(EXE):
	gcc $(CCFLAGS) -o $(EXE) main.c factorial.S string.S float.S

.PHONY: clean
clean:
	@rm -f $(EXE)
