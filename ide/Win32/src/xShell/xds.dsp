# Microsoft Developer Studio Project File - Name="xds" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=xds - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xds.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xds.mak" CFG="xds - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xds - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 VAX" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Trial" (based on "Win32 (x86) Application")
!MESSAGE "xds - Win32 Edu" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/XDSIDE/Win32/src/xShell", UNMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xds - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xds - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\objdebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "xds - Win32 VAX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\xds___W0"
# PROP BASE Intermediate_Dir ".\xds___W0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\VAX"
# PROP Intermediate_Dir ".\objvax"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USEALTICON" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /D "_USEALTICON" /D "_USEVAXLOGO" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_USEALTICON" /d "_USEVAXLOGO"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xds - Win32 Trial"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\xds___Wi"
# PROP BASE Intermediate_Dir ".\xds___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\xds___Wi"
# PROP Intermediate_Dir ".\xds___Wi"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "xds - Win32 Edu"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\xds___W0"
# PROP BASE Intermediate_Dir ".\xds___W0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\objedu"
# PROP Intermediate_Dir ".\objedu"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_EDUCVERSION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_EDUCVERSION"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib version.lib /nologo /subsystem:windows /machine:I386

!ENDIF 

# Begin Target

# Name "xds - Win32 Release"
# Name "xds - Win32 Debug"
# Name "xds - Win32 VAX"
# Name "xds - Win32 Trial"
# Name "xds - Win32 Edu"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\SRC\about.c
# End Source File
# Begin Source File

SOURCE=.\SRC\classes.c
# End Source File
# Begin Source File

SOURCE=.\SRC\config.c
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.c
# End Source File
# Begin Source File

SOURCE=.\SRC\fcache.c
# End Source File
# Begin Source File

SOURCE=.\SRC\fileutil.c
# End Source File
# Begin Source File

SOURCE=.\SRC\filter.c
# End Source File
# Begin Source File

SOURCE=.\SRC\find.c
# End Source File
# Begin Source File

SOURCE=.\SRC\flist.c
# End Source File
# Begin Source File

SOURCE=.\SRC\frame.c
# End Source File
# Begin Source File

SOURCE=.\SRC\logo.c
# End Source File
# Begin Source File

SOURCE=.\SRC\make.c
# End Source File
# Begin Source File

SOURCE=.\SRC\mdi.c
# End Source File
# Begin Source File

SOURCE=.\SRC\message.c
# End Source File
# Begin Source File

SOURCE=.\SRC\open.c
# End Source File
# Begin Source File

SOURCE=.\SRC\optedit.c
# End Source File
# Begin Source File

SOURCE=.\SRC\options.c
# End Source File
# Begin Source File

SOURCE=.\SRC\parse.c
# End Source File
# Begin Source File

SOURCE=.\SRC\pooredit.c
# End Source File
# Begin Source File

SOURCE=.\SRC\print.c
# End Source File
# Begin Source File

SOURCE=.\SRC\proj.c
# End Source File
# Begin Source File

SOURCE=.\SRC\projred.c
# End Source File
# Begin Source File

SOURCE=.\SRC\red.c
# End Source File
# Begin Source File

SOURCE=.\SRC\resource.rc
# ADD BASE RSC /l 0x419 /i "SRC"
# ADD RSC /l 0x419 /i ".\SRC" /i "SRC"
# End Source File
# Begin Source File

SOURCE=.\SRC\search.c
# End Source File
# Begin Source File

SOURCE=.\SRC\shell.c
# End Source File
# Begin Source File

SOURCE=.\SRC\syntax.c
# End Source File
# Begin Source File

SOURCE=.\SRC\tbuf.c
# End Source File
# Begin Source File

SOURCE=.\SRC\toolbar.c
# End Source File
# Begin Source File

SOURCE=.\SRC\tools.c
# End Source File
# Begin Source File

SOURCE=.\SRC\undo.c
# End Source File
# Begin Source File

SOURCE=.\SRC\util.c
# End Source File
# Begin Source File

SOURCE=.\SRC\var.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\SRC\about.h
# End Source File
# Begin Source File

SOURCE=.\SRC\classes.h
# End Source File
# Begin Source File

SOURCE=.\SRC\config.h
# End Source File
# Begin Source File

SOURCE=.\SRC\edit.h
# End Source File
# Begin Source File

SOURCE=.\SRC\fcache.h
# End Source File
# Begin Source File

SOURCE=.\SRC\fileutil.h
# End Source File
# Begin Source File

SOURCE=.\SRC\filter.h
# End Source File
# Begin Source File

SOURCE=.\SRC\find.h
# End Source File
# Begin Source File

SOURCE=.\SRC\flist.h
# End Source File
# Begin Source File

SOURCE=.\SRC\frame.h
# End Source File
# Begin Source File

SOURCE=.\SRC\logo.h
# End Source File
# Begin Source File

SOURCE=.\SRC\make.h
# End Source File
# Begin Source File

SOURCE=.\SRC\mdi.h
# End Source File
# Begin Source File

SOURCE=.\SRC\message.h
# End Source File
# Begin Source File

SOURCE=.\SRC\open.h
# End Source File
# Begin Source File

SOURCE=.\SRC\optedit.h
# End Source File
# Begin Source File

SOURCE=.\SRC\options.h
# End Source File
# Begin Source File

SOURCE=.\SRC\parse.h
# End Source File
# Begin Source File

SOURCE=.\SRC\pooredit.h
# End Source File
# Begin Source File

SOURCE=.\SRC\proj.h
# End Source File
# Begin Source File

SOURCE=.\SRC\projred.h
# End Source File
# Begin Source File

SOURCE=.\SRC\red.h
# End Source File
# Begin Source File

SOURCE=.\SRC\shell.h
# End Source File
# Begin Source File

SOURCE=.\SRC\syntax.h
# End Source File
# Begin Source File

SOURCE=.\SRC\tbuf.h
# End Source File
# Begin Source File

SOURCE=.\SRC\toolbar.h
# End Source File
# Begin Source File

SOURCE=.\SRC\tools.h
# End Source File
# Begin Source File

SOURCE=.\SRC\undo.h
# End Source File
# Begin Source File

SOURCE=.\src\util.h
# End Source File
# Begin Source File

SOURCE=.\SRC\var.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
