# Copyright (c) 1996-1998 XDS Ltd, Russia. All rights reserved.
#
# Makefile for Xm macros implementation library building
#
# $RCSfile: makefile,v $ $Revision: 2.1 $ $Date: 1998/02/14 22:29:56 $
#

SYMDIR	= sym
LIBDIR	= lib
TMPDIR	= tmp

LIBRARY	= $(LIBDIR)/libmac_Xm.a
LIB_PRJ = libmac_Xm.prj

# UNIX commands :)

MKDIR	= mkdir -p
RM	= rm -rf

XDS	= xc
XDSFLAGS= $(LIB_PRJ) =p -sym_place:=$(SYMDIR) -lib_place:=$(LIBDIR) -tmp_place:=$(TMPDIR)

.SUFFIXES: .mod .ob2 .o

.mod.o:	$*.mod
	$(XDS) $*.mod $(XDSFLAGS)

.ob2.o: $*.ob2
	$(XDS) $*.ob2 $(XDSFLAGS)

all: $(SYMDIR) $(LIBDIR) $(TMPDIR) $(LIBRARY)

$(SYMDIR):
	$(MKDIR) $(SYMDIR)

$(LIBDIR):
	$(MKDIR) $(LIBDIR)

$(TMPDIR):
	$(MKDIR) $(TMPDIR)

$(LIBRARY): $(SYMDIR) $(LIBDIR) $(TMPDIR)
	$(XDS) $(XDSFLAGS)

clean:
	$(RM) $(LIBDIR) $(SYMDIR) $(TMPDIR) $(TMP) *.h *.b *~ "#"*"#"

# Dependences

