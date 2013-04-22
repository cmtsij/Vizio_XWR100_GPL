INCLUDEDIR = .
LZMAPATH = ./lzma/C/7zip/Compress/LZMA_Lib

export HOSTCC := gcc
export HOSTCXX:= g++

CC = $(HOSTCC)
CXX = $(HOSTCXX)

CFLAGS := -I$(INCLUDEDIR) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O2

all: LZMA_LIB mksquashfs-lzma unsquashfs mksquashfs

LZMA_LIB:
	$(MAKE) -C lzma/C/7zip/Compress/LZMA_Lib all

mksquashfs: mksquashfs.o read_fs.o sort.o
	$(HOSTCC) mksquashfs.o read_fs.o sort.o -lz -o $@

mksquashfs-lzma: mksquashfs.o read_fs.o sort.o
	make -C $(LZMAPATH)
	$(CXX) mksquashfs.o read_fs.o sort.o -L$(LZMAPATH) -llzma -o $@

mksquashfs.o: mksquashfs.c squashfs_fs.h mksquashfs.h global.h sort.h

read_fs.o: read_fs.c squashfs_fs.h read_fs.h global.h

sort.o: sort.c squashfs_fs.h global.h sort.h

unsquashfs: unsquashfs.o
	$(HOSTCC) unsquashfs.o -lz -o $@

unsquashfs.o: unsquashfs.c squashfs_fs.h read_fs.h global.h

clean:
	$(MAKE) -C lzma/C/7zip/Compress/LZMA_Lib clean
	rm -f *.o mksquashfs unsquashfs mksquashfs-lzma