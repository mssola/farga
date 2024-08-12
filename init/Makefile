CROSS_CC 	?= riscv64-suse-linux-gcc
CROSS_LD 	?= riscv64-suse-linux-ld
CCFLAGS  	 = -static

##
# Targets

.PHONY: hello
hello: clean
	$(CROSS_CC) $(CCFLAGS) -o init hello.c
	cpio -o -H newc < file_list.txt > initramfs.cpio

.PHONY: shell
shell: clean
	$(CROSS_CC) $(CCFLAGS) -o init shell.c
	GOOS=linux GOARCH=riscv64 go build little_shell.go
	cpio -o -H newc < file_list.txt > initramfs.cpio

.PHONY: clean
clean:
	@rm -f *.o init initramfs.cpio
