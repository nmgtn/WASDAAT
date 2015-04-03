#
#	makefile for portsf
#
#
#	RWD OS X version January 2002
#
CC=gcc

CFLAGS=-O2 -Wall -Dunix  -I./.

PSFOBJS=	portsf.o ieee80.o

.c.o:
	$(CC) $(CFLAGS) -c $<
#
#	targets
#
all:	libportsf.a

clean:
	-rm -f copysf.o $(PSFOBJS)

veryclean:
	-rm -f copysf.o $(PSFOBJS) psfcopy

libportsf.a:	$(PSFOBJS)
	ar rc libportsf.a $(PSFOBJS)
install:	libportsf.a
	cp libportsf.a ../lib/libportsf.a
	ranlib ../lib/libportsf.a
	cp portsf.h ../include/portsf.h

#
#	dependencies
#
portsf.o:	portsf.h ieee80.h
