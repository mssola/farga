CROSS_AS 	?= riscv64-suse-linux-as
CROSS_LD 	?= riscv64-suse-linux-ld
ASFLAGS  	 = -march=rv64g -mabi=lp64

##
# Targets

.PHONY: infinite
infinite: clean
	$(CROSS_AS) $(ASFLAGS) -o infinite.o -c infinite.s
	$(CROSS_LD) -T infinite.ld infinite.o -o infinite

.PHONY: hello
hello: clean
	$(CROSS_AS) $(ASFLAGS) -o hello.o -c hello.s
	$(CROSS_LD) -T hello.ld hello.o -o hello

.PHONY: clean
clean:
	@rm -f *.o infinite hello
