# KallistiOS ##version##
#
# 2ndmix/Makefile
# Copyright (C)2003 Dan Potter
#   
# $Id: Makefile,v 1.4 2002/04/20 04:24:19 axlen Exp $

all: rm-elf 2ndmix.elf

KOS_ROMDISK_DIR = romdisk_boot
include $(KOS_BASE)/Makefile.rules

OBJS = 2ndmix.o romdisk.o

clean:
	rm -f 2ndmix.elf $(OBJS)

rm-elf:
	rm -f 2ndmix.elf
	rm -f romdisk.img

2ndmix.elf: $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o 2ndmix.elf $(KOS_START) $(OBJS) $(KOS_LIBS)

run: 2ndmix.elf
	$(KOS_LOADER) 2ndmix.elf

dist:
	rm -f $(OBJS) romdisk.*
	$(KOS_STRIP) 2ndmix.elf

