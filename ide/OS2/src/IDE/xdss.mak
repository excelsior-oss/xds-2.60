
#===================================================================
#
#   Template Make file
#   Copyright 1991 IBM Corporation
#
#===================================================================

#===================================================================
#
#   Sample application makefile, common definitions for the IBM C
#   compiler environment
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .cpp .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#
# Compile switches  that are enabled
# /c      compile don't link
# /Gm+    use the multi-threaded libraries
# /ss     allow  "//" for comment lines
# /Ms     use the system calling convention and not _optlink as the default
# /Gd-    disable optimization
# /Se     allow C Set/2  extensions
#   /Tm+ - use debug heap
#

CC         = icc /c /Gd- /Se /Re /Ms /Gm+ /Ti+ /O- /Oi- /Si+ /Wuse

LFLAGS   = /NOE /NOD /ALIGN:16 /EXEPACK /M /BASE:0x10000
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386
STLIBS  = DDE4SBS + OS2386
MTLIBS  = DDE4MBS + os2386
DLLLIBS = DDE4NBS + os2386
VLIBS   = DDE4SBS + vdh + os2386

TMPLUS  =
# /Tm+

LIBS      = $(MTLIBS)

XDSBASE_H = xdsbase.h mdi.h toolbar.h rexxcall.h ted.h redir.h bars.h fman.h fontclr.h profile.h \
 tools.h hltdll.h sf_lib.h project.h edthelp.h edthelp2.h RCThunk.h

ALL_OBJ = xdsbase.obj mdi.obj toolbar.obj bars.obj ted.obj hltdll.obj \
          project.obj sf_lib.obj redir.obj rexxcall.obj fman.obj profile.obj \
          fontclr.obj tools.obj edthelp.obj

ALL_OBJ_FUCKED_LINKER = obj\xdsbase.obj obj\mdi.obj obj\toolbar.obj obj\bars.obj obj\ted.obj obj\hltdll.obj \
                        obj\project.obj obj\sf_lib.obj obj\redir.obj obj\rexxcall.obj obj\fman.obj obj\profile.obj \
                        obj\fontclr.obj obj\tools.obj obj\edthelp.obj

all: obj\res_id.xxx

obj\nted.lnk: xdss.mak
    echo $(ALL_OBJ)         > obj\nted.lnk
    echo ..\xdss.exe       >> obj\nted.lnk
    echo xdss.map          >> obj\nted.lnk
    echo $(LIBS) + REXX    >> obj\nted.lnk
    echo nted.def          >> obj\nted.lnk

obj\xdsbase.obj:  xdsbase.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\xdsbase.obj xdsbase.cpp

obj\edthelp.obj:  edthelp.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\edthelp.obj edthelp.cpp

obj\tools.obj:     tools.cpp Filters.cpp $(XDSBASE_H) winmake\winmake.h
  $(CC) $(TMPLUS) -FoOBJ\tools.obj tools.cpp

obj\bars.obj:     bars.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\bars.obj bars.cpp

obj\fontclr.obj:  fontclr.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\fontclr.obj fontclr.cpp

obj\mdi.obj:      mdi.cpp mdi.h sf_lib.h edthelp2.h edthelp.h
  $(CC) $(TMPLUS) -FoOBJ\mdi.obj mdi.cpp

obj\toolbar.obj:  toolbar.cpp toolbar.h
  $(CC) $(TMPLUS) -FoOBJ\toolbar.obj toolbar.cpp

obj\rexxcall.obj: rexxcall.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\rexxcall.obj rexxcall.cpp

obj\ted.obj:      ted.cpp ted.h hltdll.h sf_lib.h rexxcall.h
  $(CC) -FoOBJ\ted.obj ted.cpp

obj\hltdll.obj:   hltdll.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\hltdll.obj hltdll.cpp

obj\project.obj:   project.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\project.obj project.cpp

obj\redir.obj:    redir.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\redir.obj redir.cpp

obj\sf_lib.obj:   sf_lib.cpp sf_lib.h
  $(CC) $(TMPLUS) -FoOBJ\sf_lib.obj sf_lib.cpp

obj\fman.obj:     fman.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\fman.obj fman.cpp

obj\profile.obj:  profile.cpp $(XDSBASE_H)
  $(CC) $(TMPLUS) -FoOBJ\profile.obj profile.cpp


res\xdss.res: res\xdss.rc res\xdss.dlg res\mdi.dlg $(XDSBASE_H)
  cd res
  rc -r -i .. xdss.rc
  cd ..


obj\res_id.xxx: $(ALL_OBJ_FUCKED_LINKER) obj\nted.def obj\nted.lnk res\xdss.res
    cd obj
    $(LINK) /Debug @nted.lnk
    cd ..
    rc res\xdss.res xdss.exe
    echo "Resources has been written!" >obj\res_id.xxx


