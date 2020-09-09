
.SUFFIXES:
.SUFFIXES: .obj .c .cpp 

#===================================================================
# Compile switches  that are enabled
# /c      compile don't link
# /Fi+    genreate precompiled headers
# /Si+    use precompiled headers
# /Ms     use the system calling convention and not _optlink as the default
# /Wcnd+  warn about problems in conditional statements
# /Q+     suppress logo

CC         = icc /c /Fi- /Si- /Ms /Wcnd+ /Q+ /Ti+

!IFDEF VACPP_SHARED

LFLAGS  = /NOFREE /NOE /NOD /ALIGN:16 /EXEPACK /M /BASE:0x10000  /CO
LINK    = ILINK  $(LFLAGS)
LIBS    = CPPOS30 + OS2386

!ELSE

LFLAGS  = /NOE /NOD /ALIGN:16 /EXEPACK /M /BASE:0x10000  /CO
LINK    = LINK386 $(LFLAGS)
LIBS    = DDE4SBS + OS2386

!ENDIF

.c.obj:
    $(CC) -Fo$*.obj $*.c

.cpp.obj:
    $(CC) -Fo$*.obj $*.cpp

HEADERS = basedefs.h parser.h scanner.h ir.h generate.h system.h

# --- List of all object files ---

ALL_OBJ = main.obj     parser.obj   scanner.obj  system.obj  \
          gentext.obj  genipf.obj   genrtf.obj   genhtml.obj

# --- Main target ---

all: convert.exe

# --- Linker command file ---

convert.lnk: $(ALL_OBJ) convert.mak
    @echo $(ALL_OBJ)             >  convert.lnk
    @echo convert.exe            >> convert.lnk
    @echo convert.map            >> convert.lnk
    @echo $(LIBS)                >> convert.lnk
    @echo convert.def            >> convert.lnk

# --- Object files ---

main.obj:    $(HEADERS) main.cpp
parser.obj:  $(HEADERS) parser.cpp
scanner.obj: $(HEADERS) scanner.cpp tables.cpp
ir.obj:      $(HEADERS) ir.cpp
gentext.obj: $(HEADERS) gentext.cpp
genipf.obj:  $(HEADERS) genipf.cpp
genrtf.obj:  $(HEADERS) genrtf.cpp
genhtml.obj: $(HEADERS) genhtml.cpp
system.obj:  system.h   system.cpp

# --- Executable ---

convert.exe: $(ALL_OBJ) convert.def convert.lnk
    $(LINK) @convert.lnk
