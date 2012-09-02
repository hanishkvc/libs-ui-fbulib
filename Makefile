
CCOMPILE=arm-none-linux-gnueabi-
#CCOMPILE=arm-linux-gnueabi-
#CROSS_COMPILE=
CC=$(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip
#STRIP=echo
CFLAGS=-Wall -static $(DFLAGS)
LFLAGS=-lpthread

TEMPPREFIX=/tmp/test-update

all:
	#env CROSS_COMPILE=$(CCOMPILE) DFLAGS=-DDEBUG_PRG APPPREFIX=arm-dbg- make normal
	env CROSS_COMPILE="" DFLAGS=-DDEBUG_PRG APPPREFIX=x86-dbg- make normal
	#env CROSS_COMPILE=$(CCOMPILE) APPPREFIX=arm- make normal
	env CROSS_COMPILE="" APPPREFIX=x86- make normal

normal: fbmsg

fbmsg: fbulib.c dprints.c
	$(CC) $(CFLAGS) -o $(APPPREFIX)fbmsg -DDEBUG_INFO -DPRG_SAMPLE fbulib.c

clean:
	rm -i *-dbg-* || /bin/true
	rm -i arm-* || /bin/true
	rm -i x86-* || /bin/true

