CC=gcc
CFLAGS=-fPIC -Os
AS=as
LD=ld
LDFLAGS=--gc-sections

LINK_ORDER=build/linux_x64_userspace_start.o build/linux_x64_userspace_plat_iface.o build/linux_x64_userspace_f.o

all:
	mkdir build
	$(CC) -o build/linux_x64_userspace_f.o $(CFLAGS) -c f.c
	$(AS) -o build/linux_x64_userspace_plat_iface.o plat_iface.s
	$(AS) -o build/linux_x64_userspace_start.o start.s
	$(LD) -o build/linux_x64_userspace.bin $(LDFLAGS) $(LINK_ORDER) -T binary.ld --oformat=binary

clean:
	rm -rf build
