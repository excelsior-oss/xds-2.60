# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=res_java - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to res_java - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xds - Win32 Release" && "$(CFG)" != "xds - Win32 Debug" &&\
 "$(CFG)" != "c_lang - Win32 Release" && "$(CFG)" != "c_lang - Win32 Debug" &&\
 "$(CFG)" != "m2_lang - Win32 Release" && "$(CFG)" != "m2_lang - Win32 Debug" &&\
 "$(CFG)" != "xds - Win32 VAX" && "$(CFG)" != "xds - Win32 Trial" && "$(CFG)" !=\
 "xds - Win32 Edu" && "$(CFG)" != "java_lang - Win32 Release" && "$(CFG)" !=\
 "java_lang - Win32 Debug" && "$(CFG)" != "res_mod - Win32 Release" && "$(CFG)"\
 != "res_mod - Win32 Debug" && "$(CFG)" != "res_vax - Win32 Release" && "$(CFG)"\
 != "res_vax - Win32 Debug" && "$(CFG)" != "res_mod_edu - Win32 Release" &&\
 "$(CFG)" != "res_mod_edu - Win32 Debug" && "$(CFG)" !=\
 "res_java - Win32 Release" && "$(CFG)" != "res_java - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "xds.mak" CFG="res_java - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xds - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "c_lang - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "c_lang - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m2_lang - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m2_lang - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xds - Win32 VAX" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Trial" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Edu" (based on "Win32 (x86) Application")
!MESSAGE "java_lang - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "java_lang - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_mod - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_mod - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_vax - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_vax - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_mod_edu - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_mod_edu - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_java - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "res_java - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "xds - Win32 Debug"

!IF  "$(CFG)" == "xds - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\obj

ALL : "res_vax - Win32 Release" "res_mod_edu - Win32 Release"\
 "res_mod - Win32 Release" "res_java - Win32 Release"\
 "java_lang - Win32 Release" "m2_lang - Win32 Release" "c_lang - Win32 Release"\
 "$(OUTDIR)\xds.exe"

CLEAN : 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\flist.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\logo.obj"
	-@erase "$(INTDIR)\macro.obj"
	-@erase "$(INTDIR)\make.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\open.obj"
	-@erase "$(INTDIR)\optedit.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\pooredit.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\proj.obj"
	-@erase "$(INTDIR)\projred.obj"
	-@erase "$(INTDIR)\red.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tbuf.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(OUTDIR)\xds.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
  /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xds.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/xds.pdb"\
 /machine:I386 /out:"$(OUTDIR)/xds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\flist.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\logo.obj" \
	"$(INTDIR)\macro.obj" \
	"$(INTDIR)\make.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\open.obj" \
	"$(INTDIR)\optedit.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\pooredit.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proj.obj" \
	"$(INTDIR)\projred.obj" \
	"$(INTDIR)\red.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tbuf.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\var.obj" \
	"$(OUTDIR)\c_lang.lib" \
	"$(OUTDIR)\java_lang.lib" \
	"$(OUTDIR)\m2_lang.lib" \
	"$(OUTDIR)\res_java.lib" \
	"$(OUTDIR)\res_mod.lib" \
	"$(OUTDIR)\res_mod_edu.lib" \
	"$(OUTDIR)\res_vax.lib"

"$(OUTDIR)\xds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\objdebug

ALL : "res_vax - Win32 Debug" "res_mod_edu - Win32 Debug"\
 "res_mod - Win32 Debug" "res_java - Win32 Debug" "java_lang - Win32 Debug"\
 "m2_lang - Win32 Debug" "c_lang - Win32 Debug" "$(OUTDIR)\xds.exe"\
 "$(OUTDIR)\xds.bsc"

CLEAN : 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\about.sbr"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\classes.sbr"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\config.sbr"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\edit.sbr"
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\fcache.sbr"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\fileutil.sbr"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\filter.sbr"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\find.sbr"
	-@erase "$(INTDIR)\flist.obj"
	-@erase "$(INTDIR)\flist.sbr"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\frame.sbr"
	-@erase "$(INTDIR)\logo.obj"
	-@erase "$(INTDIR)\logo.sbr"
	-@erase "$(INTDIR)\macro.obj"
	-@erase "$(INTDIR)\macro.sbr"
	-@erase "$(INTDIR)\make.obj"
	-@erase "$(INTDIR)\make.sbr"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\mdi.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\open.obj"
	-@erase "$(INTDIR)\open.sbr"
	-@erase "$(INTDIR)\optedit.obj"
	-@erase "$(INTDIR)\optedit.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\parse.sbr"
	-@erase "$(INTDIR)\pooredit.obj"
	-@erase "$(INTDIR)\pooredit.sbr"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\print.sbr"
	-@erase "$(INTDIR)\proj.obj"
	-@erase "$(INTDIR)\proj.sbr"
	-@erase "$(INTDIR)\projred.obj"
	-@erase "$(INTDIR)\projred.sbr"
	-@erase "$(INTDIR)\red.obj"
	-@erase "$(INTDIR)\red.sbr"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\shell.sbr"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\syntax.sbr"
	-@erase "$(INTDIR)\tbuf.obj"
	-@erase "$(INTDIR)\tbuf.sbr"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\toolbar.sbr"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\tools.sbr"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\undo.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(INTDIR)\var.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\xds.bsc"
	-@erase "$(OUTDIR)\xds.exe"
	-@erase "$(OUTDIR)\xds.ilk"
	-@erase "$(OUTDIR)\xds.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /FR"$(INTDIR)/"  /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug/
CPP_SBRS=.\objdebug/

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xds.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\about.sbr" \
	"$(INTDIR)\classes.sbr" \
	"$(INTDIR)\config.sbr" \
	"$(INTDIR)\edit.sbr" \
	"$(INTDIR)\fcache.sbr" \
	"$(INTDIR)\fileutil.sbr" \
	"$(INTDIR)\filter.sbr" \
	"$(INTDIR)\find.sbr" \
	"$(INTDIR)\flist.sbr" \
	"$(INTDIR)\frame.sbr" \
	"$(INTDIR)\logo.sbr" \
	"$(INTDIR)\macro.sbr" \
	"$(INTDIR)\make.sbr" \
	"$(INTDIR)\mdi.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\open.sbr" \
	"$(INTDIR)\optedit.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\parse.sbr" \
	"$(INTDIR)\pooredit.sbr" \
	"$(INTDIR)\print.sbr" \
	"$(INTDIR)\proj.sbr" \
	"$(INTDIR)\projred.sbr" \
	"$(INTDIR)\red.sbr" \
	"$(INTDIR)\search.sbr" \
	"$(INTDIR)\shell.sbr" \
	"$(INTDIR)\syntax.sbr" \
	"$(INTDIR)\tbuf.sbr" \
	"$(INTDIR)\toolbar.sbr" \
	"$(INTDIR)\tools.sbr" \
	"$(INTDIR)\undo.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\var.sbr"

"$(OUTDIR)\xds.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/xds.pdb" /debug /machine:I386 /out:"$(OUTDIR)/xds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\flist.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\logo.obj" \
	"$(INTDIR)\macro.obj" \
	"$(INTDIR)\make.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\open.obj" \
	"$(INTDIR)\optedit.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\pooredit.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proj.obj" \
	"$(INTDIR)\projred.obj" \
	"$(INTDIR)\red.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tbuf.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\var.obj" \
	"$(OUTDIR)\c_lang.lib" \
	"$(OUTDIR)\java_lang.lib" \
	"$(OUTDIR)\m2_lang.lib" \
	"$(OUTDIR)\res_java.lib" \
	"$(OUTDIR)\res_mod.lib" \
	"$(OUTDIR)\res_mod_edu.lib" \
	"$(OUTDIR)\res_vax.lib"

"$(OUTDIR)\xds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c_lang - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "c_lang\Release"
# PROP BASE Intermediate_Dir "c_lang\Release"
# PROP BASE Target_Dir "c_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj\c_lang"
# PROP Target_Dir "c_lang"
OUTDIR=.\Release
INTDIR=.\obj\c_lang

ALL : "$(OUTDIR)\c_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\c_lang.obj"
	-@erase "$(OUTDIR)\c_lang.dll"
	-@erase "$(OUTDIR)\c_lang.exp"
	-@erase "$(OUTDIR)\c_lang.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj\c_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/c_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/c_lang.pdb"\
 /machine:I386 /def:".\SRC\c\c_lang.def" /out:"$(OUTDIR)/c_lang.dll"\
 /implib:"$(OUTDIR)/c_lang.lib" 
DEF_FILE= \
	".\SRC\c\c_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\c_lang.obj"

"$(OUTDIR)\c_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "c_lang - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "c_lang\Debug"
# PROP BASE Intermediate_Dir "c_lang\Debug"
# PROP BASE Target_Dir "c_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug\c_lang"
# PROP Target_Dir "c_lang"
OUTDIR=.\Debug
INTDIR=.\objdebug\c_lang

ALL : "$(OUTDIR)\c_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\c_lang.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\c_lang.dll"
	-@erase "$(OUTDIR)\c_lang.exp"
	-@erase "$(OUTDIR)\c_lang.ilk"
	-@erase "$(OUTDIR)\c_lang.lib"
	-@erase "$(OUTDIR)\c_lang.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug\c_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/c_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/c_lang.pdb" /debug\
 /machine:I386 /def:".\SRC\c\c_lang.def" /out:"$(OUTDIR)/c_lang.dll"\
 /implib:"$(OUTDIR)/c_lang.lib" 
DEF_FILE= \
	".\SRC\c\c_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\c_lang.obj"

"$(OUTDIR)\c_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "m2_lang - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "m2_lang\Release"
# PROP BASE Intermediate_Dir "m2_lang\Release"
# PROP BASE Target_Dir "m2_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj\m2_lang"
# PROP Target_Dir "m2_lang"
OUTDIR=.\Release
INTDIR=.\obj\m2_lang

ALL : "$(OUTDIR)\m2_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\m2_lang.obj"
	-@erase "$(INTDIR)\m2_lang.res"
	-@erase "$(OUTDIR)\m2_lang.dll"
	-@erase "$(OUTDIR)\m2_lang.exp"
	-@erase "$(OUTDIR)\m2_lang.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj\m2_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/m2_lang.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/m2_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/m2_lang.pdb"\
 /machine:I386 /def:".\SRC\m2\m2_lang.def" /out:"$(OUTDIR)/m2_lang.dll"\
 /implib:"$(OUTDIR)/m2_lang.lib" 
DEF_FILE= \
	".\SRC\m2\m2_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\m2_lang.obj" \
	"$(INTDIR)\m2_lang.res"

"$(OUTDIR)\m2_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "m2_lang - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "m2_lang\Debug"
# PROP BASE Intermediate_Dir "m2_lang\Debug"
# PROP BASE Target_Dir "m2_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug\m2_lang"
# PROP Target_Dir "m2_lang"
OUTDIR=.\Debug
INTDIR=.\objdebug\m2_lang

ALL : "$(OUTDIR)\m2_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\m2_lang.obj"
	-@erase "$(INTDIR)\m2_lang.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\m2_lang.dll"
	-@erase "$(OUTDIR)\m2_lang.exp"
	-@erase "$(OUTDIR)\m2_lang.ilk"
	-@erase "$(OUTDIR)\m2_lang.lib"
	-@erase "$(OUTDIR)\m2_lang.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug\m2_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/m2_lang.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/m2_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)/m2_lang.pdb" /debug\
 /machine:I386 /def:".\SRC\m2\m2_lang.def" /out:"$(OUTDIR)/m2_lang.dll"\
 /implib:"$(OUTDIR)/m2_lang.lib" 
DEF_FILE= \
	".\SRC\m2\m2_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\m2_lang.obj" \
	"$(INTDIR)\m2_lang.res"

"$(OUTDIR)\m2_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xds___W0"
# PROP BASE Intermediate_Dir "xds___W0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VAX"
# PROP Intermediate_Dir "objvax"
# PROP Target_Dir ""
OUTDIR=.\VAX
INTDIR=.\objvax

ALL : "$(OUTDIR)\xds.exe"

CLEAN : 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\flist.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\logo.obj"
	-@erase "$(INTDIR)\macro.obj"
	-@erase "$(INTDIR)\make.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\open.obj"
	-@erase "$(INTDIR)\optedit.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\pooredit.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\proj.obj"
	-@erase "$(INTDIR)\projred.obj"
	-@erase "$(INTDIR)\red.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tbuf.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(OUTDIR)\xds.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USEALTICON" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_USEALTICON"  /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\objvax/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /D "_USEALTICON" /D "_USEVAXLOGO" /win32
MTL_PROJ=/nologo /D "NDEBUG" /D "_USEALTICON" /D "_USEVAXLOGO" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_USEALTICON" /d "_USEVAXLOGO"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "NDEBUG" /d "_USEALTICON" /d\
 "_USEVAXLOGO" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xds.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/xds.pdb"\
 /machine:I386 /out:"$(OUTDIR)/xds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\flist.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\logo.obj" \
	"$(INTDIR)\macro.obj" \
	"$(INTDIR)\make.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\open.obj" \
	"$(INTDIR)\optedit.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\pooredit.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proj.obj" \
	"$(INTDIR)\projred.obj" \
	"$(INTDIR)\red.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tbuf.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\var.obj"

"$(OUTDIR)\xds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xds___Wi"
# PROP BASE Intermediate_Dir "xds___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "xds___Wi"
# PROP Intermediate_Dir "xds___Wi"
# PROP Target_Dir ""
OUTDIR=.\xds___Wi
INTDIR=.\xds___Wi

ALL : "$(OUTDIR)\xds.exe"

CLEAN : 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\flist.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\logo.obj"
	-@erase "$(INTDIR)\macro.obj"
	-@erase "$(INTDIR)\make.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\open.obj"
	-@erase "$(INTDIR)\optedit.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\pooredit.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\proj.obj"
	-@erase "$(INTDIR)\projred.obj"
	-@erase "$(INTDIR)\red.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tbuf.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(OUTDIR)\xds.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
  /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\xds___Wi/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xds.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/xds.pdb"\
 /machine:I386 /out:"$(OUTDIR)/xds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\flist.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\logo.obj" \
	"$(INTDIR)\macro.obj" \
	"$(INTDIR)\make.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\open.obj" \
	"$(INTDIR)\optedit.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\pooredit.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proj.obj" \
	"$(INTDIR)\projred.obj" \
	"$(INTDIR)\red.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tbuf.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\var.obj"

"$(OUTDIR)\xds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xds___W0"
# PROP BASE Intermediate_Dir "xds___W0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "objedu"
# PROP Intermediate_Dir "objedu"
# PROP Target_Dir ""
OUTDIR=.\objedu
INTDIR=.\objedu

ALL : "$(OUTDIR)\xds.exe" "m2_lang - Win32 Release" "res_mod - Win32 Release"

CLEAN : 
	-@erase "$(INTDIR)\about.obj"
	-@erase "$(INTDIR)\classes.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\edit.obj"
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\fileutil.obj"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\flist.obj"
	-@erase "$(INTDIR)\frame.obj"
	-@erase "$(INTDIR)\logo.obj"
	-@erase "$(INTDIR)\macro.obj"
	-@erase "$(INTDIR)\make.obj"
	-@erase "$(INTDIR)\mdi.obj"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\open.obj"
	-@erase "$(INTDIR)\optedit.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\pooredit.obj"
	-@erase "$(INTDIR)\print.obj"
	-@erase "$(INTDIR)\proj.obj"
	-@erase "$(INTDIR)\projred.obj"
	-@erase "$(INTDIR)\red.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\shell.obj"
	-@erase "$(INTDIR)\syntax.obj"
	-@erase "$(INTDIR)\tbuf.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\undo.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(OUTDIR)\xds.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_EDUCVERSION" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_EDUCVERSION"  /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\objedu/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_EDUCVERSION"
# SUBTRACT RSC /x
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/resource.res" /d "NDEBUG" /d "_EDUCVERSION" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/xds.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib\
 version.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/xds.pdb"\
 /machine:I386 /out:"$(OUTDIR)/xds.exe" 
LINK32_OBJS= \
	"$(INTDIR)\about.obj" \
	"$(INTDIR)\classes.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\edit.obj" \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\fileutil.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\flist.obj" \
	"$(INTDIR)\frame.obj" \
	"$(INTDIR)\logo.obj" \
	"$(INTDIR)\macro.obj" \
	"$(INTDIR)\make.obj" \
	"$(INTDIR)\mdi.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\open.obj" \
	"$(INTDIR)\optedit.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\pooredit.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proj.obj" \
	"$(INTDIR)\projred.obj" \
	"$(INTDIR)\red.obj" \
	"$(INTDIR)\resource.res" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\shell.obj" \
	"$(INTDIR)\syntax.obj" \
	"$(INTDIR)\tbuf.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\undo.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\var.obj"

"$(OUTDIR)\xds.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "java_lang - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "java_lang\Release"
# PROP BASE Intermediate_Dir "java_lang\Release"
# PROP BASE Target_Dir "java_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj\m2_lang"
# PROP Target_Dir "java_lang"
OUTDIR=.\Release
INTDIR=.\obj\java_lang

ALL : "$(OUTDIR)\java_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\java_lang.obj"
	-@erase "$(OUTDIR)\java_lang.dll"
	-@erase "$(OUTDIR)\java_lang.exp"
	-@erase "$(OUTDIR)\java_lang.lib"
	-@erase ".\obj\java_lang\java_lang.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj\m2_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"obj\java_lang\java_lang.res" /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"obj\java_lang\java_lang.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/java_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/java_lang.pdb" /machine:I386 /def:".\SRC\java\java_lang.def"\
 /out:"$(OUTDIR)/java_lang.dll" /implib:"$(OUTDIR)/java_lang.lib" 
DEF_FILE= \
	".\SRC\java\java_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\java_lang.obj" \
	".\obj\java_lang\java_lang.res"

"$(OUTDIR)\java_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "java_lang\Debug"
# PROP BASE Intermediate_Dir "java_lang\Debug"
# PROP BASE Target_Dir "java_lang"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug\java_lang"
# PROP Target_Dir "java_lang"
OUTDIR=.\Debug
INTDIR=.\objdebug\java_lang

ALL : "$(OUTDIR)\java_lang.dll"

CLEAN : 
	-@erase "$(INTDIR)\java_lang.obj"
	-@erase "$(INTDIR)\java_lang.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\java_lang.dll"
	-@erase "$(OUTDIR)\java_lang.exp"
	-@erase "$(OUTDIR)\java_lang.ilk"
	-@erase "$(OUTDIR)\java_lang.lib"
	-@erase "$(OUTDIR)\java_lang.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug\java_lang/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/java_lang.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/java_lang.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/java_lang.pdb" /debug /machine:I386\
 /def:".\SRC\java\java_lang.def" /out:"$(OUTDIR)/java_lang.dll"\
 /implib:"$(OUTDIR)/java_lang.lib" 
DEF_FILE= \
	".\SRC\java\java_lang.def"
LINK32_OBJS= \
	"$(INTDIR)\java_lang.obj" \
	"$(INTDIR)\java_lang.res"

"$(OUTDIR)\java_lang.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_mod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "res_mod\Release"
# PROP BASE Intermediate_Dir "res_mod\Release"
# PROP BASE Target_Dir "res_mod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir "res_mod"
OUTDIR=.\Release
INTDIR=.\obj

ALL : "$(OUTDIR)\res_mod.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_mod.res"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_mod.dll"
	-@erase "$(OUTDIR)\res_mod.exp"
	-@erase "$(OUTDIR)\res_mod.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /x /d "NDEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_mod.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_mod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/res_mod.pdb" /machine:I386 /def:".\SRC\res_mod.def"\
 /out:"$(OUTDIR)/res_mod.dll" /implib:"$(OUTDIR)/res_mod.lib" 
DEF_FILE= \
	".\SRC\res_mod.def"
LINK32_OBJS= \
	"$(INTDIR)\res_mod.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_mod.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_mod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "res_mod\Debug"
# PROP BASE Intermediate_Dir "res_mod\Debug"
# PROP BASE Target_Dir "res_mod"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug"
# PROP Target_Dir "res_mod"
OUTDIR=.\Debug
INTDIR=.\objdebug

ALL : "$(OUTDIR)\res_mod.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_mod.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_mod.dll"
	-@erase "$(OUTDIR)\res_mod.exp"
	-@erase "$(OUTDIR)\res_mod.ilk"
	-@erase "$(OUTDIR)\res_mod.lib"
	-@erase "$(OUTDIR)\res_mod.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /x /d "_DEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_mod.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_mod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/res_mod.pdb" /debug /machine:I386 /def:".\SRC\res_mod.def"\
 /out:"$(OUTDIR)/res_mod.dll" /implib:"$(OUTDIR)/res_mod.lib" 
DEF_FILE= \
	".\SRC\res_mod.def"
LINK32_OBJS= \
	"$(INTDIR)\res_mod.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_mod.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_vax - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "res_vax\Release"
# PROP BASE Intermediate_Dir "res_vax\Release"
# PROP BASE Target_Dir "res_vax"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir "res_vax"
OUTDIR=.\Release
INTDIR=.\obj

ALL : "$(OUTDIR)\res_vax.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_vax.res"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_vax.dll"
	-@erase "$(OUTDIR)\res_vax.exp"
	-@erase "$(OUTDIR)\res_vax.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /x /d "NDEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_vax.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_vax.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/res_vax.pdb" /machine:I386 /def:".\SRC\res_vax.def"\
 /out:"$(OUTDIR)/res_vax.dll" /implib:"$(OUTDIR)/res_vax.lib" 
DEF_FILE= \
	".\SRC\res_vax.def"
LINK32_OBJS= \
	"$(INTDIR)\res_vax.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_vax.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_vax - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "res_vax\Debug"
# PROP BASE Intermediate_Dir "res_vax\Debug"
# PROP BASE Target_Dir "res_vax"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug"
# PROP Target_Dir "res_vax"
OUTDIR=.\Debug
INTDIR=.\objdebug

ALL : "$(OUTDIR)\res_vax.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_vax.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_vax.dll"
	-@erase "$(OUTDIR)\res_vax.exp"
	-@erase "$(OUTDIR)\res_vax.ilk"
	-@erase "$(OUTDIR)\res_vax.lib"
	-@erase "$(OUTDIR)\res_vax.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /x /d "_DEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_vax.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_vax.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/res_vax.pdb" /debug /machine:I386 /def:".\SRC\res_vax.def"\
 /out:"$(OUTDIR)/res_vax.dll" /implib:"$(OUTDIR)/res_vax.lib" 
DEF_FILE= \
	".\SRC\res_vax.def"
LINK32_OBJS= \
	"$(INTDIR)\res_vax.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_vax.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "res_mod_edu\Release"
# PROP BASE Intermediate_Dir "res_mod_edu\Release"
# PROP BASE Target_Dir "res_mod_edu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir "res_mod_edu"
OUTDIR=.\Release
INTDIR=.\obj

ALL : "$(OUTDIR)\res_mod_edu.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_mod_edu.res"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_mod_edu.dll"
	-@erase "$(OUTDIR)\res_mod_edu.exp"
	-@erase "$(OUTDIR)\res_mod_edu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /x /d "NDEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_mod_edu.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_mod_edu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/res_mod_edu.pdb" /machine:I386 /def:".\SRC\res_mod_edu.def"\
 /out:"$(OUTDIR)/res_mod_edu.dll" /implib:"$(OUTDIR)/res_mod_edu.lib" 
DEF_FILE= \
	".\SRC\res_mod_edu.def"
LINK32_OBJS= \
	"$(INTDIR)\res_mod_edu.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_mod_edu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "res_mod_edu\Debug"
# PROP BASE Intermediate_Dir "res_mod_edu\Debug"
# PROP BASE Target_Dir "res_mod_edu"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug"
# PROP Target_Dir "res_mod_edu"
OUTDIR=.\Debug
INTDIR=.\objdebug

ALL : "$(OUTDIR)\res_mod_edu.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_mod_edu.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_mod_edu.dll"
	-@erase "$(OUTDIR)\res_mod_edu.exp"
	-@erase "$(OUTDIR)\res_mod_edu.ilk"
	-@erase "$(OUTDIR)\res_mod_edu.lib"
	-@erase "$(OUTDIR)\res_mod_edu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /x /d "_DEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_mod_edu.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_mod_edu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/res_mod_edu.pdb" /debug /machine:I386\
 /def:".\SRC\res_mod_edu.def" /out:"$(OUTDIR)/res_mod_edu.dll"\
 /implib:"$(OUTDIR)/res_mod_edu.lib" 
DEF_FILE= \
	".\SRC\res_mod_edu.def"
LINK32_OBJS= \
	"$(INTDIR)\res_mod_edu.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_mod_edu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_java - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "res_java\Release"
# PROP BASE Intermediate_Dir "res_java\Release"
# PROP BASE Target_Dir "res_java"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir "res_java"
OUTDIR=.\Release
INTDIR=.\obj

ALL : "$(OUTDIR)\res_java.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_java.res"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_java.dll"
	-@erase "$(OUTDIR)\res_java.exp"
	-@erase "$(OUTDIR)\res_java.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\obj/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /x /d "NDEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_java.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_java.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/res_java.pdb" /machine:I386 /def:".\SRC\res_java.def"\
 /out:"$(OUTDIR)/res_java.dll" /implib:"$(OUTDIR)/res_java.lib" 
DEF_FILE= \
	".\SRC\res_java.def"
LINK32_OBJS= \
	"$(INTDIR)\res_java.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_java.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "res_java - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "res_java\Debug"
# PROP BASE Intermediate_Dir "res_java\Debug"
# PROP BASE Target_Dir "res_java"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "objdebug"
# PROP Target_Dir "res_java"
OUTDIR=.\Debug
INTDIR=.\objdebug

ALL : "$(OUTDIR)\res_java.dll"

CLEAN : 
	-@erase "$(INTDIR)\res_java.res"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\xdsres.obj"
	-@erase "$(OUTDIR)\res_java.dll"
	-@erase "$(OUTDIR)\res_java.exp"
	-@erase "$(OUTDIR)\res_java.ilk"
	-@erase "$(OUTDIR)\res_java.lib"
	-@erase "$(OUTDIR)\res_java.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\objdebug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /x /d "_DEBUG"
RSC_PROJ=/l 0x409 /x /fo"$(INTDIR)/res_java.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/res_java.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/res_java.pdb" /debug /machine:I386 /nodefaultlib\
 /def:".\SRC\res_java.def" /out:"$(OUTDIR)/res_java.dll"\
 /implib:"$(OUTDIR)/res_java.lib" 
DEF_FILE= \
	".\SRC\res_java.def"
LINK32_OBJS= \
	"$(INTDIR)\res_java.res" \
	"$(INTDIR)\xdsres.obj"

"$(OUTDIR)\res_java.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "xds - Win32 Release"
# Name "xds - Win32 Debug"
# Name "xds - Win32 VAX"
# Name "xds - Win32 Trial"
# Name "xds - Win32 Edu"

!IF  "$(CFG)" == "xds - Win32 Release"

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\var.c
DEP_CPP_VAR_C=\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\var.sbr" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\util.c
DEP_CPP_UTIL_=\
	".\SRC\fileutil.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\util.sbr" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\undo.c
DEP_CPP_UNDO_=\
	".\SRC\undo.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\undo.sbr" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\undo.obj" : $(SOURCE) $(DEP_CPP_UNDO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\tools.c

!IF  "$(CFG)" == "xds - Win32 Release"

DEP_CPP_TOOLS=\
	".\SRC\classes.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\tools.obj" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

DEP_CPP_TOOLS=\
	".\SRC\classes.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tools.obj" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tools.sbr" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

DEP_CPP_TOOLS=\
	".\SRC\classes.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\tools.obj" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

DEP_CPP_TOOLS=\
	".\SRC\classes.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\tools.obj" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

DEP_CPP_TOOLS=\
	".\SRC\classes.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\tools.obj" : $(SOURCE) $(DEP_CPP_TOOLS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\toolbar.c
DEP_CPP_TOOLB=\
	".\SRC\config.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\toolbar.sbr" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\tbuf.c
DEP_CPP_TBUF_=\
	".\SRC\tbuf.h"\
	".\SRC\tbuf_impl.h"\
	".\SRC\undo.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\tbuf.obj" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\tbuf.obj" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\tbuf.sbr" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\tbuf.obj" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\tbuf.obj" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\tbuf.obj" : $(SOURCE) $(DEP_CPP_TBUF_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\syntax.c
DEP_CPP_SYNTA=\
	".\SRC\config.h"\
	".\SRC\fileutil.h"\
	".\src\lang.h"\
	".\SRC\shell.h"\
	".\SRC\syntax.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\syntax.sbr" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\syntax.obj" : $(SOURCE) $(DEP_CPP_SYNTA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\shell.c
DEP_CPP_SHELL=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\frame.h"\
	".\src\lang.h"\
	".\SRC\macro.h"\
	".\SRC\logo.h"\
	".\SRC\make.h"\
	".\SRC\mdi.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\optedit.h"\
	".\SRC\proj.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\syntax.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	".\SRC\xdsres.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\shell.sbr" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\shell.obj" : $(SOURCE) $(DEP_CPP_SHELL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\search.c
DEP_CPP_SEARC=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\flist.h"\
	".\SRC\frame.h"\
	".\SRC\mdi.h"\
	".\SRC\open.h"\
	".\SRC\red.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\red.c
DEP_CPP_RED_C=\
	".\SRC\red.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\red.obj" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\red.obj" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\red.sbr" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\red.obj" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\red.obj" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\red.obj" : $(SOURCE) $(DEP_CPP_RED_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\projred.c
DEP_CPP_PROJR=\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\options.h"\
	".\SRC\parse.h"\
	".\SRC\projred.h"\
	".\SRC\red.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\projred.obj" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\projred.obj" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\projred.sbr" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\projred.obj" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\projred.obj" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\projred.obj" : $(SOURCE) $(DEP_CPP_PROJR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\proj.c
DEP_CPP_PROJ_=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\optedit.h"\
	".\SRC\options.h"\
	".\SRC\parse.h"\
	".\SRC\proj.h"\
	".\SRC\projred.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\proj.obj" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\proj.obj" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\proj.sbr" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\proj.obj" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\proj.obj" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\proj.obj" : $(SOURCE) $(DEP_CPP_PROJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\print.c
DEP_CPP_PRINT=\
	".\SRC\config.h"\
	".\SRC\fileutil.h"\
	".\SRC\pooredit.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\print.sbr" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\print.obj" : $(SOURCE) $(DEP_CPP_PRINT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\pooredit.c
DEP_CPP_POORE=\
	".\src\lang.h"\
	".\src\macro.h"\
	".\SRC\pooredit.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\syntax.h"\
	".\SRC\tbuf.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\pooredit.obj" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\pooredit.obj" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\pooredit.sbr" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\pooredit.obj" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\pooredit.obj" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\pooredit.obj" : $(SOURCE) $(DEP_CPP_POORE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\parse.c
DEP_CPP_PARSE=\
	".\SRC\fcache.h"\
	".\SRC\message.h"\
	".\SRC\options.h"\
	".\SRC\parse.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\winmake.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\parse.sbr" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\options.c
DEP_CPP_OPTIO=\
	".\SRC\fileutil.h"\
	".\SRC\options.h"\
	".\SRC\shell.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\options.sbr" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\options.obj" : $(SOURCE) $(DEP_CPP_OPTIO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\optedit.c
DEP_CPP_OPTED=\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fileutil.h"\
	".\SRC\open.h"\
	".\SRC\optedit.h"\
	".\SRC\options.h"\
	".\SRC\parse.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\optedit.obj" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\optedit.obj" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\optedit.sbr" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\optedit.obj" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\optedit.obj" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\optedit.obj" : $(SOURCE) $(DEP_CPP_OPTED) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\open.c
DEP_CPP_OPEN_=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fileutil.h"\
	".\SRC\open.h"\
	".\SRC\projred.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\open.obj" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\open.obj" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\open.sbr" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\open.obj" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\open.obj" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\open.obj" : $(SOURCE) $(DEP_CPP_OPEN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\message.c
DEP_CPP_MESSA=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\frame.h"\
	".\SRC\mdi.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\winmake.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\message.sbr" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\message.obj" : $(SOURCE) $(DEP_CPP_MESSA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\mdi.c
DEP_CPP_MDI_C=\
	".\SRC\mdi.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\mdi.sbr" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\mdi.obj" : $(SOURCE) $(DEP_CPP_MDI_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\make.c

!IF  "$(CFG)" == "xds - Win32 Release"

DEP_CPP_MAKE_=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\make.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

DEP_CPP_MAKE_=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\make.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\make.sbr" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

DEP_CPP_MAKE_=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\make.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

DEP_CPP_MAKE_=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\make.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

DEP_CPP_MAKE_=\
	".\SRC\about.h"\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\flist.h"\
	".\SRC\make.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\winmake.h"\
	

"$(INTDIR)\make.obj" : $(SOURCE) $(DEP_CPP_MAKE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\logo.c
DEP_CPP_LOGO_=\
	".\SRC\classes.h"\
	".\SRC\logo.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\src\util.h"\
	".\SRC\xdsres.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\logo.obj" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\logo.obj" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\logo.sbr" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\logo.obj" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\logo.obj" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\logo.obj" : $(SOURCE) $(DEP_CPP_LOGO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\macro.c
DEP_CPP_MACRO_=\
	".\SRC\macro.h"\
	".\SRC\util.h"\
	".\SRC\var.h"\
	".\SRC\reshelp.h"\
	".\SRC\fileutil.h"\
	".\src\shell.h"\
	".\SRC\syntax.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\macro.obj" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\macro.obj" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\macro.sbr" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\macro.obj" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\macro.obj" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\macro.obj" : $(SOURCE) $(DEP_CPP_MACRO_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\frame.c
DEP_CPP_FRAME=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\frame.h"\
	".\SRC\logo.h"\
	".\SRC\mdi.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\frame.sbr" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\frame.obj" : $(SOURCE) $(DEP_CPP_FRAME) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\flist.c
DEP_CPP_FLIST=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\flist.h"\
	".\SRC\frame.h"\
	".\SRC\mdi.h"\
	".\SRC\open.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\flist.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\flist.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\flist.sbr" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\flist.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\flist.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\flist.obj" : $(SOURCE) $(DEP_CPP_FLIST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\filter.c
DEP_CPP_FILTE=\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\filter.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	".\SRC\winmake.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\filter.sbr" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\fileutil.c
DEP_CPP_FILEU=\
	".\SRC\fileutil.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fileutil.sbr" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\fileutil.obj" : $(SOURCE) $(DEP_CPP_FILEU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\fcache.c
DEP_CPP_FCACH=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\shell.h"\
	".\SRC\tbuf.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\fcache.obj" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\fcache.obj" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\fcache.sbr" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\fcache.obj" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\fcache.obj" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\fcache.obj" : $(SOURCE) $(DEP_CPP_FCACH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\edit.c
DEP_CPP_EDIT_=\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\fcache.h"\
	".\SRC\fileutil.h"\
	".\SRC\find.h"\
	".\SRC\frame.h"\
	".\src\lang.h"\
	".\SRC\mdi.h"\
	".\SRC\message.h"\
	".\SRC\open.h"\
	".\SRC\pooredit.h"\
	".\SRC\proj.h"\
	".\SRC\projred.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\syntax.h"\
	".\SRC\tbuf.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\edit.sbr" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\edit.obj" : $(SOURCE) $(DEP_CPP_EDIT_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\config.c
DEP_CPP_CONFI=\
	".\SRC\classes.h"\
	".\SRC\config.h"\
	".\SRC\edit.h"\
	".\SRC\flist.h"\
	".\src\lang.h"\
	".\SRC\make.h"\
	".\SRC\mdi.h"\
	".\SRC\message.h"\
	".\SRC\pooredit.h"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\SRC\syntax.h"\
	".\SRC\tbuf.h"\
	".\SRC\toolbar.h"\
	".\SRC\tools.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\xdsres.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\config.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\config.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\classes.c
DEP_CPP_CLASS=\
	".\SRC\classes.h"\
	".\SRC\shell.h"\
	".\src\util.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\classes.sbr" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\about.c
DEP_CPP_ABOUT=\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\shell.h"\
	".\src\util.h"\
	".\SRC\var.h"\
	".\SRC\xdsres.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\about.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\about.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\about.sbr" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\about.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\about.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\about.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\resource.rc
DEP_RSC_RESOU=\
	".\SRC\closed.bmp"\
	".\SRC\compil.bmp"\
	".\SRC\console.ico"\
	".\SRC\copy1.bmp"\
	".\SRC\cut.bmp"\
	".\SRC\debug.bmp"\
	".\SRC\edit.ico"\
	".\SRC\find.bmp"\
	".\SRC\findnext.bmp"\
	".\SRC\flist.ico"\
	".\SRC\handcls.cur"\
	".\SRC\help1.bmp"\
	".\SRC\hsize.cur"\
	".\SRC\item.bmp"\
	".\SRC\make.bmp"\
	".\SRC\makeall.bmp"\
	".\SRC\move_any.cur"\
	".\SRC\move_d.cur"\
	".\SRC\move_dl.cur"\
	".\SRC\move_dr.cur"\
	".\SRC\move_l.cur"\
	".\SRC\move_r.cur"\
	".\SRC\move_u.cur"\
	".\SRC\move_ul.cur"\
	".\SRC\move_ur.cur"\
	".\SRC\msg.ico"\
	".\SRC\new.bmp"\
	".\SRC\open.bmp"\
	".\SRC\opened.bmp"\
	".\SRC\paste.bmp"\
	".\SRC\res.h"\
	".\SRC\reshelp.h"\
	".\SRC\run2.bmp"\
	".\SRC\save.bmp"\
	".\SRC\scroll.ico"\
	".\SRC\stop.bmp"\
	".\SRC\undo.bmp"\
	".\SRC\user.bmp"\
	".\SRC\vsize.cur"\
	".\SRC\xds.ico"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/resource.res" /i "SRC" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/resource.res" /i "SRC" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/resource.res" /i "SRC" /d "NDEBUG" /d\
 "_USEALTICON" /d "_USEVAXLOGO" $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/resource.res" /i "SRC" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\resource.res" : $(SOURCE) $(DEP_RSC_RESOU) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/resource.res" /i "SRC" /d "NDEBUG" /d\
 "_EDUCVERSION" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "c_lang"

!IF  "$(CFG)" == "xds - Win32 Release"

"c_lang - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="c_lang - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

"c_lang - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="c_lang - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "m2_lang"

!IF  "$(CFG)" == "xds - Win32 Release"

"m2_lang - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="m2_lang - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

"m2_lang - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="m2_lang - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

"m2_lang - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="m2_lang - Win32 Release" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=.\SRC\find.c
DEP_CPP_FIND_=\
	".\SRC\find.h"\
	".\SRC\tbuf.h"\
	".\SRC\tbuf_impl.h"\
	".\SRC\undo.h"\
	

!IF  "$(CFG)" == "xds - Win32 Release"


"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\find.sbr" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"


"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Trial"


"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "xds - Win32 Edu"


"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Project Dependency

# Project_Dep_Name "java_lang"

!IF  "$(CFG)" == "xds - Win32 Release"

"java_lang - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="java_lang - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

"java_lang - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="java_lang - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "res_java"

!IF  "$(CFG)" == "xds - Win32 Release"

"res_java - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_java - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

"res_java - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_java - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "res_mod"

!IF  "$(CFG)" == "xds - Win32 Debug"

"res_mod - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_mod - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 Release"

"res_mod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_mod - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

"res_mod - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_mod - Win32 Release" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "res_mod_edu"

!IF  "$(CFG)" == "xds - Win32 Debug"

"res_mod_edu - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_mod_edu - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 Release"

"res_mod_edu - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_mod_edu - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

# End Project Dependency
################################################################################
# Begin Project Dependency

# Project_Dep_Name "res_vax"

!IF  "$(CFG)" == "xds - Win32 Debug"

"res_vax - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_vax - Win32 Debug" 

!ELSEIF  "$(CFG)" == "xds - Win32 Release"

"res_vax - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\xds.mak" CFG="res_vax - Win32 Release" 

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

!ENDIF 

# End Project Dependency
# End Target
################################################################################
# Begin Target

# Name "c_lang - Win32 Release"
# Name "c_lang - Win32 Debug"

!IF  "$(CFG)" == "c_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "c_lang - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\c\c_lang.c

!IF  "$(CFG)" == "c_lang - Win32 Release"

DEP_CPP_C_LAN=\
	".\src\lang.h"\
	".\src\util.h"\
	

"$(INTDIR)\c_lang.obj" : $(SOURCE) $(DEP_CPP_C_LAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "c_lang - Win32 Debug"

DEP_CPP_C_LAN=\
	".\src\lang.h"\
	".\src\util.h"\
	

"$(INTDIR)\c_lang.obj" : $(SOURCE) $(DEP_CPP_C_LAN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\c\c_lang.def

!IF  "$(CFG)" == "c_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "c_lang - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "m2_lang - Win32 Release"
# Name "m2_lang - Win32 Debug"

!IF  "$(CFG)" == "m2_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "m2_lang - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\m2\m2_lang.def

!IF  "$(CFG)" == "m2_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "m2_lang - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\m2\m2_lang.c

!IF  "$(CFG)" == "m2_lang - Win32 Release"

DEP_CPP_M2_LA=\
	".\src\lang.h"\
	".\src\util.h"\
	

"$(INTDIR)\m2_lang.obj" : $(SOURCE) $(DEP_CPP_M2_LA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "m2_lang - Win32 Debug"

DEP_CPP_M2_LA=\
	".\src\lang.h"\
	".\src\util.h"\
	

"$(INTDIR)\m2_lang.obj" : $(SOURCE) $(DEP_CPP_M2_LA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\m2\m2_lang.rc

!IF  "$(CFG)" == "m2_lang - Win32 Release"


"$(INTDIR)\m2_lang.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/m2_lang.res" /i "SRC\m2" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "m2_lang - Win32 Debug"


"$(INTDIR)\m2_lang.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/m2_lang.res" /i "SRC\m2" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "java_lang - Win32 Release"
# Name "java_lang - Win32 Debug"

!IF  "$(CFG)" == "java_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\java\resource.h

!IF  "$(CFG)" == "java_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\java\java_lang.def

!IF  "$(CFG)" == "java_lang - Win32 Release"

!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\java\java_lang.rc

!IF  "$(CFG)" == "java_lang - Win32 Release"


".\obj\java_lang\java_lang.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"obj\java_lang\java_lang.res" /i "SRC\java" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"


"$(INTDIR)\java_lang.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x409 /fo"$(INTDIR)/java_lang.res" /i "SRC\java" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\java\java_lang.c

!IF  "$(CFG)" == "java_lang - Win32 Release"

NODEP_CPP_JAVA_=\
	".\SRC\java\lang.h"\
	".\SRC\java\util.h"\
	

"$(INTDIR)\java_lang.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "java_lang - Win32 Debug"

NODEP_CPP_JAVA_=\
	".\SRC\java\lang.h"\
	".\SRC\java\util.h"\
	

"$(INTDIR)\java_lang.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "res_mod - Win32 Release"
# Name "res_mod - Win32 Debug"

!IF  "$(CFG)" == "res_mod - Win32 Release"

!ELSEIF  "$(CFG)" == "res_mod - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\xdsres.c

!IF  "$(CFG)" == "res_mod - Win32 Release"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "res_mod - Win32 Debug"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_mod.rc

!IF  "$(CFG)" == "res_mod - Win32 Release"

DEP_RSC_RES_M=\
	".\SRC\about.bmp"\
	".\SRC\logo.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_mod.res" : $(SOURCE) $(DEP_RSC_RES_M) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_mod.res" /i "SRC" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "res_mod - Win32 Debug"

DEP_RSC_RES_M=\
	".\SRC\about.bmp"\
	".\SRC\logo.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_mod.res" : $(SOURCE) $(DEP_RSC_RES_M) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_mod.res" /i "SRC" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_mod.def

!IF  "$(CFG)" == "res_mod - Win32 Release"

!ELSEIF  "$(CFG)" == "res_mod - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "res_vax - Win32 Release"
# Name "res_vax - Win32 Debug"

!IF  "$(CFG)" == "res_vax - Win32 Release"

!ELSEIF  "$(CFG)" == "res_vax - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\xdsres.c

!IF  "$(CFG)" == "res_vax - Win32 Release"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "res_vax - Win32 Debug"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_vax.rc

!IF  "$(CFG)" == "res_vax - Win32 Release"

DEP_RSC_RES_V=\
	".\SRC\aboutvax.bmp"\
	".\SRC\logovax.bmp"\
	".\SRC\red_icon.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_vax.res" : $(SOURCE) $(DEP_RSC_RES_V) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_vax.res" /i "SRC" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "res_vax - Win32 Debug"

DEP_RSC_RES_V=\
	".\SRC\aboutvax.bmp"\
	".\SRC\logovax.bmp"\
	".\SRC\red_icon.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_vax.res" : $(SOURCE) $(DEP_RSC_RES_V) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_vax.res" /i "SRC" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_vax.def

!IF  "$(CFG)" == "res_vax - Win32 Release"

!ELSEIF  "$(CFG)" == "res_vax - Win32 Debug"

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "res_mod_edu - Win32 Release"
# Name "res_mod_edu - Win32 Debug"

!IF  "$(CFG)" == "res_mod_edu - Win32 Release"

!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\res_mod_edu.def

!IF  "$(CFG)" == "res_mod_edu - Win32 Release"

!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\xdsres.c

!IF  "$(CFG)" == "res_mod_edu - Win32 Release"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Debug"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_mod_edu.rc

!IF  "$(CFG)" == "res_mod_edu - Win32 Release"

DEP_RSC_RES_MO=\
	".\SRC\aboutedu.bmp"\
	".\SRC\logoedu.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_mod_edu.res" : $(SOURCE) $(DEP_RSC_RES_MO) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_mod_edu.res" /i "SRC" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "res_mod_edu - Win32 Debug"

DEP_RSC_RES_MO=\
	".\SRC\aboutedu.bmp"\
	".\SRC\logoedu.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_mod_edu.res" : $(SOURCE) $(DEP_RSC_RES_MO) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_mod_edu.res" /i "SRC" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "res_java - Win32 Release"
# Name "res_java - Win32 Debug"

!IF  "$(CFG)" == "res_java - Win32 Release"

!ELSEIF  "$(CFG)" == "res_java - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\SRC\res_java.rc

!IF  "$(CFG)" == "res_java - Win32 Release"

DEP_RSC_RES_J=\
	".\SRC\about.bmp"\
	".\SRC\logo.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_java.res" : $(SOURCE) $(DEP_RSC_RES_J) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_java.res" /i "SRC" /d "NDEBUG"\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "res_java - Win32 Debug"

DEP_RSC_RES_J=\
	".\SRC\about.bmp"\
	".\SRC\logo.bmp"\
	".\SRC\xds.ico"\
	".\SRC\xdsres.h"\
	

"$(INTDIR)\res_java.res" : $(SOURCE) $(DEP_RSC_RES_J) "$(INTDIR)"
   $(RSC) /l 0x409 /x /fo"$(INTDIR)/res_java.res" /i "SRC" /d "_DEBUG"\
 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\res_java.def

!IF  "$(CFG)" == "res_java - Win32 Release"

!ELSEIF  "$(CFG)" == "res_java - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\SRC\xdsres.c

!IF  "$(CFG)" == "res_java - Win32 Release"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "res_java - Win32 Debug"


"$(INTDIR)\xdsres.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
