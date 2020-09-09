
#===================================================================
#
#   Template Make file
#   Copyright 1991 IBM Corporation
#
#===================================================================

.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .cpp .asm .hlp .itl .ipf

CC         = icc /c /Gd- /Ge- /Se /Re /Ms /Gm+ /Ti+ /Si+ /Wuse

LFLAGS   = /NOE /NOD /ALIGN:16 /EXEPACK /M /BASE:0x10000
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386
STLIBS  = DDE4SBS + OS2386
MTLIBS  = DDE4MBS + os2386
DLLLIBS = DDE4NBS + os2386
VLIBS   = DDE4SBS + vdh + os2386

LIBS      = $(MTLIBS)

ALL_OBJ   = obj\hltc.obj ..\obj\sf_lib.obj

all: hltc.dll

obj\xdscpp.lnk: xdscppdll.mak
    echo $(ALL_OBJ)         > obj\xdscpp.lnk
    echo hltc.dll          >> obj\xdscpp.lnk
    echo obj\hltc.map      >> obj\xdscpp.lnk
    echo $(LIBS)           >> obj\xdscpp.lnk
    echo obj\xdsdll.def    >> obj\xdscpp.lnk

obj\hltc.obj: hlt_cpp.cpp ..\hltdll.h ..\sf_lib.h
  $(CC) -FoOBJ\hltc.obj hlt_cpp.cpp


hltc.dll: $(ALL_OBJ) obj\xdscpp.lnk
    $(LINK) /Debug @obj\xdscpp.lnk



