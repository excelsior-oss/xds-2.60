XDS_HOME = /usr/local/xdsn230

LIBDIR = lib
XDS_LIBDIR = $(XDS_HOME)/lib/X86
XDS_SYMDIR = $(XDS_HOME)/sym/X86

X11_LIB = libmac_X11.a
XM_LIB = libmac_Xm.a

RM = rm -f
INSTALL = install -c
MKDIR = mkdir -p

all:	syms libs

libs:	$(LIBDIR)/$(X11_LIB) $(LIBDIR)/$(XM_LIB)

$(LIBDIR)/$(X11_LIB): Xlibsyms
	cd macro/X11; xc =p libmac_X11.prj

$(LIBDIR)/$(XM_LIB): Xmsyms
	cd macro/Xm; xc =p libmac_Xm.prj

syms:	Xlibsyms Xmsyms

Xlibsyms:	X11.prj
	$(MKDIR) sym lib
	xc =p X11.prj

Xmsyms:	XmP.prj
	$(MKDIR) sym lib
	xc =p XmP.prj

install: all
	$(INSTALL) -d $(XDS_SYMDIR)/X11
	$(INSTALL) -d $(XDS_LIBDIR)/X11
	$(INSTALL) -m 644 $(LIBDIR)/$(X11_LIB) $(XDS_LIBDIR)/X11
	$(INSTALL) -m 644 $(LIBDIR)/$(XM_LIB) $(XDS_LIBDIR)/X11
	$(INSTALL) -m 644 sym/*.sym $(XDS_SYMDIR)/X11

clean:
	cd sym; $(RM) *.sym
	$(RM) $(LIBDIR)/$(X11_LIB) $(LIBDIR)/$(XM_LIB)
	cd macro/X11; make clean -f makefile.lib
	cd macro/Xm; make clean -f makefile.lib


