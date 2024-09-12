obj-m += dtm.o

# Build either from a local kernel or from the running system.
KERNEL ?=
ifeq ($(strip $(KERNEL)),)
	DIR=/lib/modules/$$(uname -r)/build
else
	DIR="$(KERNEL)"
endif

all:
	make -C $(DIR) M=$(PWD) modules

clean:
	make -C $(DIR) M=$(PWD) clean
