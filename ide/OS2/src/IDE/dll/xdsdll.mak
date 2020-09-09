
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

ALL_OBJ   = obj\hltm.obj ..\obj\sf_lib.obj

all: hltm.dll

obj\xdsmod.lnk: xdsdll.mak
    echo $(ALL_OBJ)         > obj\xdsmod.lnk
    echo hltm.dll          >> obj\xdsmod.lnk
    echo obj\hltm.map      >> obj\xdsmod.lnk
    echo $(LIBS)           >> obj\xdsmod.lnk
    echo obj\xdsdll.def    >> obj\xdsmod.lnk

obj\hltm.obj: hlt_mod.cpp ..\hltdll.h ..\sf_lib.h
  $(CC) -FoOBJ\hltm.obj hlt_mod.cpp


hltm.dll: $(ALL_OBJ) obj\xdsmod.lnk
    $(LINK) /Debug @obj\xdsmod.lnk


