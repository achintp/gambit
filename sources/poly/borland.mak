#
# $Source$
# $Date$
# $Revision$
#
# DESCRIPTION:
# Makefile for Borland C++ 5.5 for poly module
#

.AUTODEPEND

!include ..\makedef.bcc

EXTRACPPFLAGS = -v -I$(BCCDIR)\include -I.. -D__BCC55__
EXTRALINKFLAGS = -Tpe -aa -v -V4.0 -c

libpoly_a_SOURCES = \
	gpartltr.cc \
	gpoly.cc \
	gpolylst.cc \
	gsolver.cc \
	ideal.cc \
	ineqsolv.cc \
	interval.cc \
	monomial.cc \
	pelclass.cc \
	poly.cc \
	prepoly.cc \
	quiksolv.cc \
	rectangl.cc

OBJECTS = $(libpoly_a_SOURCES:.cc=.obj)

CFG = ..\gambit32.cfg

OPT = -Od

.cc.obj:
	bcc32 $(CPPFLAGS) -P -c {$< }

LINKFLAGS= /Tpe /L$(WXLIBDIR);$(BCCDIR)\lib $(EXTRALINKFLAGS)
OPT = -Od
DEBUG_FLAGS= -v


CPPFLAGS= $(WXINC) $(EXTRACPPFLAGS) $(OPT) @$(CFG)

poly: $(OBJECTS)
        copy *.obj ..

clean:
        -erase *.obj
        -erase *.exe
        -erase *.res
        -erase *.map
        -erase *.rws

