CC      = $(CROSS_COMPILE)gcc
CCFLAGS = -static
EXE     = basics

DEBUG =
ifneq ($(strip $(DEBUG)),)
	CCFLAGS += -g
endif

.PHONY: all
all: clean $(EXE)

.PHONY: $(EXE)
$(EXE):
	$(CC) $(CCFLAGS) -o $(EXE) basics.c factorial.S string.S float.S

.PHONY: clean
clean:
	@rm -f $(EXE)
