# MODE = ENDUSER | WORK

!ifndef MODE
MODE=ENDUSER
!message MODE is not defined - assuming ENDUSER.
!endif

LIB_OPTIONS     = -dbgfmt=HLL -decor=rt -woff+
DLL_OPTIONS     = -dbgfmt=HLL -decor=rt -woff+
ARCHIVE         = xds-260-win32.zip
ARCHIVE_DBG     = xds-260-debug-win32.zip

!if "$(OS)" == "Windows_NT"
SYS     = $(WINDIR)\system32
!else
SYS     = $(WINDIR)\system
!endif

# Target files

MERGE           = enduser\xds\def\win32\build\merge.exe
REEXPORT        = enduser\xds\def\win32\build\reexport.exe
WINDEF          = enduser\xds\def\win32\Windows.def
RMSYM           = rmsym.bat

LIBXDS          = libxds.lib
LIBTS           = libts.lib
MSVCMKF         = enduser\xds\lib\C\msvc.mkf
MSVCTSMKF       = enduser\xds\lib\C\msvcts.mkf
WATCOMMKF       = enduser\xds\lib\C\watcom.mkf
WATCOMTSMKF     = enduser\xds\lib\C\watcomts.mkf
UNIXMKF         = enduser\xds\lib\C\unix.mkf
UNIXTSMKF       = enduser\xds\lib\C\unixts.mkf
CLANGMKF        = enduser\xds\lib\C\clangcl.mkf
CLANGTSMKF      = enduser\xds\lib\C\clangclts.mkf

XM              = enduser\xds\bin\xm.exe

IMPORT32        = enduser\xds\lib\x86\import32.lib
EXESTARTUP      = enduser\xds\lib\x86\xstart.lib
DLLSTARTUP      = enduser\xds\lib\x86\xstartd.lib
CSTARTUP        = enduser\xds\lib\x86\xstartc.lib
XSTARTUP        = enduser\xds\lib\x86\xstartx.lib
STLIB           = enduser\xds\lib\x86\libxds.lib 
MTLIB           = enduser\xds\lib\x86\libxdsmt.lib 
STDLL           = enduser\xds\bin\xds26.dll
MTDLL           = enduser\xds\bin\xds26m.dll 
TSSTLIB         = enduser\xds\lib\x86\libts.lib
TSMTLIB         = enduser\xds\lib\x86\libtsmt.lib
TSSTDLL         = enduser\xds\bin\xts26.dll
TSMTDLL         = enduser\xds\bin\xts26m.dll
STIMPLIB        = enduser\xds\lib\x86\xds26i.lib
MTIMPLIB        = enduser\xds\lib\x86\xds26mi.lib
TSSTIMPLIB      = enduser\xds\lib\x86\xts26i.lib
TSMTIMPLIB      = enduser\xds\lib\x86\xts26mi.lib
VTERMLIB        = enduser\xds\lib\x86\vterm.lib

WINSYM_X86      = enduser\xds\sym\x86\Windows.sym
WINSYM_C        = enduser\xds\sym\c\Windows.sym
POSIXSYM_C      = enduser\xds\sym\C\stdio.sym

XC              = enduser\xds\bin\xc.exe

XD_UTL          = enduser\xds\bin\xd_utl.dll 
XD_DITLS        = enduser\xds\bin\xd_ditls.dll 
XD_NB04         = enduser\xds\bin\xd_nb04.dll
XD_NB09         = enduser\xds\bin\xd_nb09.dll 
XD_EDIF         = enduser\xds\bin\xd_edif.dll 
XD              = enduser\xds\bin\xd.exe 
XD_SRV          = enduser\xds\bin\xd_srv.exe
XD_DEMON        = enduser\xds\bin\xd_demon.exe
XD_TCP          = enduser\xds\bin\xd_t_tcp.dll
HIS             = enduser\xds\bin\his.exe 

XPROF           = enduser\xds\bin\xprof.exe 
XPROFMEM        = enduser\xds\bin\xprofmem.exe 
XPROFAPI        = enduser\xds\bin\xprofapi.dll
XPDUMP          = enduser\xds\bin\xpdump.exe 
XSTRIP          = enduser\xds\bin\xstrip.exe 
XDASM           = enduser\xds\bin\xdasm.exe
XLIB            = enduser\xds\bin\xlib.exe
XPVIEW          = enduser\xds\bin\xpview.exe
XRC             = enduser\xds\bin\xrc.exe
XLINK           = enduser\xds\bin\xlink.exe
H2D             = enduser\xds\bin\h2d.exe

XDS             = enduser\xds\bin\xds.exe
XDSRES          = enduser\xds\bin\xdsres.dll
CLANG           = enduser\xds\bin\c_lang.dll
JAVALANG        = enduser\xds\bin\java_lang.dll
M2LANG          = enduser\xds\bin\m2_lang.dll

CONVERTN        = doc\convertn.exe

XMHLP           = enduser\xds\bin\xm.hlp
XMCNT           = enduser\xds\bin\xm.cnt
XMPDF           = enduser\xds\pdf\xm.pdf

XCHLP           = enduser\xds\bin\xc.hlp
XCCNT           = enduser\xds\bin\xc.cnt
XCPDF           = enduser\xds\pdf\xc.pdf

XDHLP           = enduser\xds\bin\xd.hlp
XDCNT           = enduser\xds\bin\xd.cnt
XDPDF           = enduser\xds\pdf\xd.pdf

O2REPHLP        = enduser\xds\bin\o2rep.hlp
O2REPCNT        = enduser\xds\bin\o2rep.cnt
O2REPPDF        = enduser\xds\pdf\o2rep.pdf

H2DHLP          = enduser\xds\bin\h2d.hlp
H2DCNT          = enduser\xds\bin\h2d.cnt
H2DPDF          = enduser\xds\pdf\h2d.pdf

XDSLIBHLP       = enduser\xds\bin\xdslib.hlp
XDSLIBCNT       = enduser\xds\bin\xdslib.cnt
XDSLIBPDF       = enduser\xds\pdf\xdslib.pdf
XDSENVHLP       = enduser\xds\bin\xdsenv.hlp
XDSENVCNT       = enduser\xds\bin\xdsenv.cnt
XDSHLP          = enduser\xds\bin\xds.hlp


$(ARCHIVE_DBG): all
        @if not exist $(XDSDIR)\bin\xds26.dll  copy $(XDSDIR)\bin\xds25.dll  enduser\xds\bin
        @if not exist $(XDSDIR)\bin\xds26m.dll copy $(XDSDIR)\bin\xds25m.dll enduser\xds\bin
        @if exist $(ARCHIVE) del $(ARCHIVE)
        cd enduser
        zip -r ..\$(ARCHIVE) xds
        cd ..
        @if exist $(ARCHIVE_DBG) del $(ARCHIVE_DBG)
        cd debug
        zip -r ..\$(ARCHIVE_DBG) *
        cd ..
        @echo ====== XDS 2.6 $(MODE) version successfully built =====
        @echo ====== XDS $(MODE) version prepared for distributive creation ======

all:                \
        win32_api   \
        xm_lib      \
        xm_comp     \
        xc_lib      \
        xc_comp     \
        xd          \
        xprof       \
        tools       
# NOTE: IDE generation dosabled
#        ide         
# NOTE: Help generation disabled 
#        ide_doc     \
#        tools_doc   \
#        lang_doc    \
#        lib_doc     \
#        xc_doc      \
#        xd_doc      \
#        xm_doc      \
#        xds_doc     
        @echo.
        @echo ================ XDS 2.6 $(MODE) version successfully built ============
        @echo.
        @echo.


win32_api:           \
        $(MERGE)     \
        $(REEXPORT)  \
        $(RMSYM)
        @echo ====== Win32 API successfully built =====
        @echo.
        @echo.


xm_lib:                                \
        $(LIBXDS)       $(LIBTS)       \
        $(MSVCMKF)      $(MSVCTSMKF)   \
        $(WATCOMMKF)    $(WATCOMTSMKF) \
        $(UNIXMKF)      $(UNIXTSMKF)   \
        $(CLANGMKF)     $(CLANGTSMKF)  \
        $(WINSYM_C)     $(POSIXSYM_C)                      
        @echo ====== XDS-C Libraries successfully built =====
        @echo.
        @echo.

xm_comp:          \
        $(XM)     
        @echo ====== XDS-C Compiler successfully built =====
        @echo.
        @echo.


xc_lib:                                 \
        $(IMPORT32)                     \
        $(EXESTARTUP)   $(DLLSTARTUP)   \
        $(STLIB)        $(MTLIB)        \
        $(STDLL)        $(MTDLL)        \
        $(TSSTLIB)      $(TSMTLIB)      \
        $(TSSTDLL)      $(TSMTDLL)      \
        $(STIMPLIB)     $(MTIMPLIB)     \
        $(TSSTIMPLIB)   $(TSMTIMPLIB)   \
        $(VTERMLIB) 
        @echo ====== XDS-x86 Libraries successfully built =====
        @echo.
        @echo.

xc_comp:          \
        $(XC)     
        @echo ====== XDS-x86 Compiler successfully built =====
        @echo.
        @echo.


xd:                                  \
        $(XD_UTL)       $(XD_DITLS)  \
        $(XD_NB04)      $(XD_NB09)   \
        $(XD_EDIF)                   \
        $(XD)                        \
        $(XD_SRV)       $(XD_TCP)    \
        $(XD_DEMON)                  \
        $(HIS)      
        @echo ====== XDS-x86 Debugger successfully built =====
        @echo.
        @echo.


xprof:                               \
        $(XPROF)        $(XPROFMEM)  \
        $(XPROFAPI)                  \
        $(XPVIEW)
        @echo ====== XProf successfully built =====
        @echo.
        @echo.

tools:              \
        $(XPDUMP)   \
        $(XSTRIP)   \
        $(XDASM)    \
        $(XLIB)     \
        $(H2D)      \
        $(XRC)      \
        $(XLINK)    
        @echo ====== XDS-x86 Tools successfully built =====
        @echo.
        @echo.


ide:            \
        $(XDS)     
        @echo ====== XDS-x86 IDE successfully built =====
        @echo.
        @echo.


ide_doc:             \
        $(XDSENVHLP)
        @echo ====== IDE Documentation successfully built =====
        @echo.
        @echo.

tools_doc:             \
        $(H2DHLP)      \
        $(H2DPDF)       
        @echo ====== Tools Documentation successfully built =====
        @echo.
        @echo.

lang_doc:            \
        $(O2REPHLP)  \
        $(O2REPPDF)
        @echo ====== Language Documentation successfully built =====
        @echo.
        @echo.


lib_doc:                              \
        $(ISOLIBHLP)    $(ISOLIBPDF)  \
        $(XDSLIBHLP)    $(XDSLIBPDF)  
        @echo ====== Libraries Documentation successfully built =====
        @echo.
        @echo.

xc_doc:            \
        $(XCHLP)   \
        $(XCPDF)  
        @echo ====== XDS-x86 Compiler Documentation successfully built =====
        @echo.
        @echo.

xd_doc:          \
        $(XDHLP) \
        $(XDPDF)
        @echo ====== XDS-x86 Debugger Documentation successfully built =====
        @echo.
        @echo.

xm_doc:            \
        $(XMHLP)   \
        $(XMPDF)   
        @echo ====== XDS-C Debugger Documentation successfully built =====
        @echo.
        @echo.

xds_doc:          \
        $(XDSHLP)  
        @echo ====== XDS Documentation successfully built =====
        @echo.
        @echo.


#=============================================================================
# Build Win32 API
#=============================================================================
$(MERGE):
        cl src\win32\build\merge.c /Foenduser\tmp\merge.obj /Fesrc\win32\build\merge.exe  1>log\merge.out 2>&1
        copy src\win32\build\merge.exe $(MERGE)                                           1>>log\merge.out 2>>&1
        @echo ====== $@ =========== OK!
        @echo.

$(REEXPORT):
        cl src\win32\build\reexport.c /Foenduser\tmp\reexport.obj /Fesrc\win32\build\reexport.exe  1>log\reexport.out 2>&1
        copy src\win32\build\reexport.exe $(REEXPORT)                                              1>>log\reexport.out 2>>&1
        @echo ====== $@ =========== OK!
        @echo.

$(WINDEF): $(M) $(MERGE) $(REEXPORT)
        cd src\win32
        build\merge                            1>..\..\log\windef.out 2>&1
        cd ..\..
        copy src\win32\Windows.def $(WINDEF)   1>>log\windef.out 2>>&1
        @echo ====== WINDOWS.DEF ======== OK!
        @echo.

$(RMSYM): $(M) $(WINDEF)
        if exist $@ del $@
        xc =g enduser\xds\def\win32\windef.prj -genremovescript:+  1>log\rmsym.out 2>&1
        @echo ====== RMSYM.BAT ======== OK!
        @echo.


$(WINSYM_X86): $(M) $(MTDLL)
        xc =p enduser\xds\def\win32\windef.prj $(LIB_OPTIONS)      1>log\winsym_x86.out 2>&1
        @echo ====== Win32 x86 sym-files ======== OK!
        @echo.

$(WINSYM_C): $(M) $(MTDLL)
        xm =p enduser\xds\def\win32\windef.prj $(LIB_OPTIONS)      1>log\winsym_c.out 2>&1
        @echo ====== Win32 C sym-files ======== OK!
        @echo.


$(POSIXSYM_C): $(M)
        xm =p enduser\xds\def\posix\posix.prj $(LIB_OPTIONS)      1>log\posixsym_c.out 2>&1
        @echo ====== POSIX C sym-files ======== OK!
        @echo.

#=============================================================================
# Build XDS-C Labraries
#=============================================================================
$(LIBXDS): $(WINDEF_XM)
        @if exist enduser\xds\lib\c\libxds.lib del enduser\xds\lib\c\libxds.lib
        -@del /q enduser\tmp\*                          1>nul 2>nul
        -@del /q enduser\xds\sym\c\*                    1>nul 2>nul
        -@del /q enduser\xds\C\C\*                      1>nul 2>nul
        -@del /q enduser\xds\include\C\*                1>nul 2>nul
        xcopy src\os\posix\*.c enduser\xds\C\C\         1>log\libxds.out  2>&1 
        xcopy src\os\posix\*.h enduser\xds\include\C\   1>>log\libxds.out 2>>&1
        xcopy src\xr\c\*.c enduser\xds\C\C\             1>>log\libxds.out 2>>&1
        xcopy src\xr\c\*.h enduser\xds\include\C\       1>>log\libxds.out 2>>&1
        xcopy src\Win32\*.h enduser\xds\include\C\      1>>log\libxds.out 2>>&1
        xm src\m\lib -mode:=nodebug =p =a $(LIB_OPTIONS) -env_target=winnt -env_host=winnt  1>>log\libxds.out 2>>&1
        copy enduser\xds\lib\C\libxds.lib               1>>log\libxds.out 2>>&1
        -@del enduser\xds\lib\C\lib.mkf                 1>nul 2>nul
        -@del enduser\xds\lib\C\libxds.*                1>nul 2>nul
        @echo ====== LIBXDS.LIB ====== OK!
        @echo.

$(LIBTS): $(LIBXDS) $(WINDEF_XM)
        @if exist enduser\xds\lib\c\libts.lib del enduser\xds\lib\c\libts.lib
        -@del /q enduser\tmp\*                          1>nul 2>nul
        xcopy src\TSlibs\POSIX\*.c enduser\xds\C\C\     1>log\libts.out  2>&1
        copy src\TSlibs\IO.* src\TSlibs\IO_.*           1>>log\libts.out 2>>&1
        copy enduser\xds\def\ts\IO.def enduser\xds\def\ts\IO_.def  1>>log\libts.out 2>>&1
        xm src\tslibs\tslib -mode:=nodebug =p =a $(LIB_OPTIONS) -env_target=winnt -env_host=winnt  1>>log\libts.out 2>>&1
        copy enduser\xds\lib\C\libts.lib                1>>log\libts.out 2>>&1
        -@del enduser\xds\lib\C\tslib.mkf               1>nul 2>nul
        -@del enduser\xds\lib\C\libts.*                 1>nul 2>nul
        @echo ====== LIBTS.LIB ====== OK!
        @echo.

$(MSVCMKF): $(LIBXDS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\m\lib =g -mode:=user -mkf_name:=msvc -env_target=winnt -cc=MSVC 1>..\..\..\..\log\msvcmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== MSVC.MKF ======== OK!
        @echo.

$(MSVCTSMKF): $(LIBTS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\tslibs\tslib =g -mode:=user -mkf_name:=msvcts -env_target=winnt -cc=MSVC 1>..\..\..\..\log\msvctsmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== MSVCTS.MKF ======== OK!
        @echo.

$(WATCOMMKF): $(LIBXDS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\m\lib =g -mode:=user -mkf_name:=watcom -env_target=watcomnt -cc=Watcom 1>..\..\..\..\log\watcommkf.out 2>&1
        cd ..\..\..\..
        @echo ====== WATCOM.MKF ======== OK!
        @echo.

$(WATCOMTSMKF): $(LIBTS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\tslibs\tslib =g -mode:=user -mkf_name:=watcomts -env_target=watcomnt -cc=Watcom 1>..\..\..\..\log\msvctsmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== WATCOMTS.MKF ======== OK!
        @echo.

$(UNIXMKF): $(LIBXDS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\m\lib =g -mode:=user -mkf_name:=unix -env_target= -target_fs=unix -target_family=unix -target_os=unix -libext=a  1>..\..\..\..\log\unixmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== UNIX.MKF ======== OK!
        @echo.

$(UNIXTSMKF): $(LIBTS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\tslibs\tslib =g -mode:=user -mkf_name:=unixts -env_target= -target_fs=unix -target_family=unix -target_os=unix -libext=a -escape_io_h:+ 1>..\..\..\..\log\unixtsmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== MSVCTS.MKF ======== OK!
        @echo.

$(CLANGMKF): $(LIBXDS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\m\lib =g -mode:=user -mkf_name:=clangcl -env_target=clangnt -cc=CLangCL 1>..\..\..\..\log\clangclmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== MSVC.MKF ======== OK!
        @echo.

$(CLANGTSMKF): $(LIBTS)
        if exist $@ del $@
        cd enduser\xds\lib\c
        xm ..\..\..\..\src\tslibs\tslib =g -mode:=user -mkf_name:=clangclts -env_target=clangnt -cc=CLangCL 1>..\..\..\..\log\clangcltsmkf.out 2>&1
        cd ..\..\..\..
        @echo ====== MSVCTS.MKF ======== OK!
        @echo.


#=============================================================================
# Build XDS-C Compiler
#=============================================================================
$(XM): $(LIBXDS)
        -del /q enduser\tmp
        xm src\m\xm =p -mode:=enduser -ts_compatible:+ $(LIB_OPTIONS) -env_target=winnt -env_host=winnt 1>log\xm.out 2>&1
        @echo ====== XM.EXE ============ OK!
        @echo.



#=============================================================================
# Build XDS-x86 Labraries
#=============================================================================
$(IMPORT32):
#set sys=%windir%\system
#if exist %windir%\system32\advapi32.dll set sys=%windir%\system32
#set win=%windir%
#
#rem %win%\NDDEAPI.dll  !!! Is NE executable
#rem %win%\MGMTAPI.dll  !!! not found
#rem $(SYS)\URL.dll      !!! not found
#rem $(SYS)\OPENGL32.dll !!! not found
#rem $(SYS)\GLU32.dll    !!! not found
#rem %win%\DLCAPI.dll   !!! not found
#rem $(SYS)\WIN32SPL.dll !!! not found
#rem $(SYS)\WINSPOOL.dll !!! not found
#
#rem $(SYS)\MPR.dll      !!! separate lib (DllGetClassObject, DllCanUnloadNow)
#rem $(SYS)\MAPI32.DLL   !!! separate lib (DllGetClassObject, DllCanUnloadNow)
#rem $(SYS)\AVIFIL32.DLL !!! separate lib (DllGetClassObject, DllCanUnloadNow)
#
#rem $(SYS)\VERSION.dll  !!! separate lib (VerLanguageNameW/...A with KERNEL32.dll)
#
#rem $(SYS)\RPCRT4.dll   !!! separate lib (RpcIfIdVectorFree with RPCNS4.dll)
#rem $(SYS)\RPCNS4.dll   !!! separate lib (RpcIfIdVectorFree with RPCRT4.dll)
#
#rem $(SYS)\RASAPI32.dll !!! absent in NT4.0 and in Windows.def
#rem $(SYS)\PKPD32.dll   !!! absent in NT4.0 and in Windows.def
#rem %win%\INETMIB1.DLL !!! absent in NT4.0 and in Windows.def
#rem $(SYS)\WOW32.DLL    !!! absent in NT4.0 and in Windows.def
        xlib /implib enduser\xds\lib\x86\import32 $(SYS)\ADVAPI32.dll $(SYS)\KERNEL32.dll $(SYS)\LZ32.dll $(SYS)\NETAPI32.dll $(SYS)\VDMDBG.dll $(SYS)\WSOCK32.dll $(SYS)\COMCTL32.DLL $(SYS)\COMDLG32.dll $(SYS)\CTL3D32.dll $(SYS)\GDI32.dll $(SYS)\SHELL32.DLL $(SYS)\USER32.dll $(SYS)\WINMM.dll $(SYS)\IMM32.dll $(SYS)\OLEDLG.dll $(SYS)\TAPI32.DLL $(SYS)\AVICAP32.DLL $(SYS)\MSACM32.dll $(SYS)\MSVFW32.DLL $(SYS)\WINSPOOL.DRV 1>log\import.out 2>&1
        @echo ====== IMPORT32.LIB ====== OK!
        @echo.

$(EXESTARTUP):
        @if exist $@ del $@
        cd src\os\w95\startup
        nmake /f startup.mkf MODE=$(MODE) 1>..\..\..\..\log\exestartup.out 2>&1
        cd ..\..\..\..
        @echo ====== XSTART.LIB ====== OK!
        @echo.

$(DLLSTARTUP):
        @if exist $@ del $@
        cd src\os\w95\startup
        nmake /f startup.mkf MODE=$(MODE)  1>..\..\..\..\log\dllstartup.out 2>&1
        cd ..\..\..\..
        @echo ====== XSTARTD.LIB ====== OK!
        @echo.


$(STLIB): $(WINDEF)
        -@del /q enduser\tmp
        -@del /q enduser\xds\sym\x86\*
        @if exist $@ del $@
        xc src\m\lib -mode:=debug =p =all -multithread- $(LIB_OPTIONS)  1>log\stlib_dbg.out  2>&1
        copy $@ debug\$(@F)                                             1>>log\stlib_dbg.out 2>>&1
!if ("$(MODE)" == "ENDUSER")
        -@del /q enduser\tmp
        -@del /q enduser\xds\sym\x86\*
        @if exist $@ del $@
        xc src\m\lib -mode:=nodebug =p =all -multithread- $(LIB_OPTIONS) 1>log\stlib.out  2>&1
!endif
        @echo ====== LIBXDS.LIB ======== OK!
        @echo.

$(MTLIB): $(STLIB) 
        -@del /q enduser\tmp\*
        -@del /q enduser\xds\sym\x86\*
        @if exist $@ del $@
        xc src\m\lib -mode:=debug =p =all -multithread+ $(LIB_OPTIONS)   1>log\mtlib_dbg.out 2>&1
        copy $@ debug\$(@F)                                              
!if ("$(MODE)" == "ENDUSER")
        -@del /q enduser\tmp
        -@del /q enduser\xds\sym\x86\*
        @if exist $@ del $@
        xc src\m\lib -mode:=nodebug =p =all -multithread+ $(LIB_OPTIONS) 1>log\mtlib.out 2>&1
!endif
        @echo ====== LIBXDSMT.LIB ====== OK!
        @echo.

$(STDLL): $(MTLIB) $(DLLSTARTUP) $(IMPORT32)
        -@del /q enduser\tmp\*
        -@del /q enduser\xds\sym\x86\*
        xc src\m\lib -mode:=debug =p =all -gendll+ -multithread- $(DLL_OPTIONS)  1>log\stdll.out 2>&1
        copy $@ debug\$(@F)
        @xstrip -q -n -t $@
        @echo ====== XDS26.DLL ====== OK!
        @echo.

$(MTDLL): $(STDLL) $(DLLSTARTUP) $(IMPORT32) $(RMSYM)
        -@del /q enduser\tmp\*
        -@del /q enduser\xds\sym\x86\*
        xc src\m\lib -mode:=debug =p =all -gendll+ -multithread+ $(DLL_OPTIONS)  1>log\mtdll.out 2>&1
        copy $@ debug\$(@F)
        xstrip -q -n -t $@                                                       1>>log\mtdll.out 2>>&1
        $(RMSYM)                                                                 1>>log\mtdll.out 2>>&1
        @echo ====== XDS26M.DLL ====== OK!
        @echo.

$(STIMPLIB): $(STDLL) $(WINSYM_X86)
        @if exist $@ del $@
        xlib /implib $(STIMPLIB) $(STDLL)                                   1>log\stimplib.out 2>&1
        cd src\os\w95
        nmake /a /f linkinto.mkf MODE=$(MODE) XC_OPTIONS="$(DLL_OPTIONS)"   1>>..\..\..\log\stimplib.out 2>>&1
        cd    ..\..\..
#        wlib -b -c $(STIMPLIB) +li.lib                                      1>>log\stimplib.out 2>>&1
        xlib /nobak $(STIMPLIB) +li.lib                                      1>>log\stimplib.out 2>>&1
        @echo ====== XDS26I.LIB ======== OK!
        @echo.

$(MTIMPLIB): $(MTDLL) $(WINSYM_X86)
        @if exist $@ del $@
        xlib /implib $(MTIMPLIB) $(MTDLL)                                  1>log\mtimplib.out 2>&1
        cd src\os\w95
        nmake /a /f linkinto.mkf MODE=$(MODE) XC_OPTIONS="$(DLL_OPTIONS)"  1>>..\..\..\log\mtimplib.out 2>>&1
        cd    ..\..\..
#        wlib -b -c $(MTIMPLIB) +li.lib                                     1>>log\mtimplib.out 2>>&1
        xlib /nobak $(MTIMPLIB) +li.lib                                     1>>log\mtimplib.out 2>>&1
        @echo ====== XDS26MI.LIB ======== OK!
        @echo.

$(TSSTLIB): $(MTIMPLIB)
        -@del /q enduser\tmp
        @if exist $@ del $@
        xc src\tslibs\tslib -mode:=debug =p =all -multithread- $(LIB_OPTIONS)    1>>log\tsstlib_dbg.out 2>>&1
        copy $@ debug\$(@F)                                                      1>>log\tsstlib_dbg.out 2>>&1
!if ("$(MODE)" == "ENDUSER")
        -@del /q enduser\tmp
        @if exist $@ del $@
        xc src\tslibs\tslib -mode:=nodebug =p =all -multithread- $(LIB_OPTIONS)  1>log\tsstlib.out 2>&1
!endif
        @echo ====== LIBTS.LIB ======== OK!
        @echo.

$(TSMTLIB): $(TSSTLIB) 
        -@del /q enduser\tmp\*
        @if exist $@ del $@
        xc src\tslibs\tslib -mode:=debug =p =all -multithread+ $(LIB_OPTIONS)    1>log\tsmtlib_dbg.out 2>&1
        copy $@ debug\$(@F)                                                      1>>log\tsmtlib_dbg.out 2>>&1
!if ("$(MODE)" == "ENDUSER")
        -@del /q enduser\tmp\*
        @if exist $@ del $@
        xc src\tslibs\tslib -mode:=nodebug =p =all -multithread+ $(LIB_OPTIONS)  1>log\tsmtlib.out 2>&1
!endif
        @echo ====== LIBTSMT.LIB ====== OK!
        @echo.

$(TSSTDLL): $(TSMTLIB) $(STIMPLIB) $(DLLSTARTUP) $(IMPORT32)
        -@del /q enduser\tmp\*
        xc src\tslibs\tslib -mode:=debug =p =all -gendll+ -multithread- $(DLL_OPTIONS)  1>log\tsstdll.out 2>&1
        copy $@ debug\$(@F)                                                             1>>log\tsstdll.out 2>>&1
        xstrip -q -n -t $@                                                              1>>log\tsstdll.out 2>>&1
        @echo ====== XTS26.DLL ====== OK!
        @echo.

$(TSMTDLL): $(TSSTDLL) $(MTIMPLIB) $(DLLSTARTUP) $(IMPORT32)
        -@del /q enduser\tmp\*
        xc src\tslibs\tslib -mode:=debug =p =all -gendll+ -multithread+ $(DLL_OPTIONS)  1>log\tsmtdll.out 2>&1
        copy $@ debug\$(@F)                                                             1>>log\tsmtdll.out 2>>&1
        xstrip -q -n -t $@                                                              1>>log\tsmtdll.out 2>>&1
        @echo ====== XTS26M.DLL ====== OK!
        @echo.

$(TSSTIMPLIB): $(TSSTDLL)
        @if exist $@ del $@
        xlib /implib $@ $(TSSTDLL)   1>log\tsstimplib.out 2>&1
        @echo ====== XTS26I.LIB ======== OK!
        @echo.

$(TSMTIMPLIB): $(TSMTDLL)
        @if exist $@ del $@
        xlib /implib $@ $(TSMTDLL)   1>log\tsmtimplib.out 2>&1
        @echo ====== XTS26MI.LIB ======== OK!
        @echo.

$(VTERMLIB): $(WINSYM_X86)
        @if exist $@ del $@
        -@del /q enduser\tmp
        xc =p src\vterm\vterm -mode:=debug $(LIB_OPTIONS)    1> log\vtermlib.out 2>&1
        copy $@ debug\$(@F)                                  1>>log\vtermlib.out 2>>&1
        xstrip -q -n -t $@                                   1>>log\vtermlib.out 2>>&1
        @echo ====== VTERM.LIB ============ OK!
        @echo.


#=============================================================================
# Build XDS-x86 Compiler
#=============================================================================
$(XC): $(EXESTARTUP) $(STLIB) $(WINSYM_X86)
        -@del /q enduser\tmp
        xc src\m\xc =p =a -mode:=enduser -ts_compatible:+ $(LIB_OPTIONS)  1>log\xc.out 2>&1
        copy $@ debug\$(@F)                                               1>>log\xc.out 2>>&1
        xstrip -q -n -t $@                                                1>>log\xc.out 2>>&1
        @echo ====== XC.EXE ============ OK!
        @echo.


#=============================================================================
# Build Debuger
#=============================================================================
$(XD_UTL): $(DLLSTARTUP) $(STIMPLIB) $(WINSYM_X86)
        -@del /q enduser\tmp
        xc =p src\xd_prj\xd.prj -mode:=enduser -component:=xd_utl $(DLL_OPTIONS)  1>log\xd_utl.out 2>&1
        copy $@ debug\$(@F)                                                       1>>log\xd_utl.out 2>>&1
        xstrip -q -n -t $@                                                        1>>log\xd_utl.out 2>>&1
        @echo ====== XD_UTL.DLL ============ OK!
        @echo.

$(XD_DITLS): $(DLLSTARTUP) $(STIMPLIB) $(XD_UTL) $(WINSYM_X86)
        xc =p src\xd_prj\xd.prj -mode:=enduser -component:=xd_ditls $(DLL_OPTIONS)  1>log\xd_ditls.out 2>&1  
        copy $@ debug\$(@F)                                                         1>>log\xd_ditls.out 2>>&1
        xstrip -q -n -t $@                                                          1>>log\xd_ditls.out 2>>&1
        @echo ====== XD_DITLS.DLL ============ OK!
        @echo.

$(XD_NB04): $(DLLSTARTUP) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86)
        xc =p src\xd_prj\xd.prj -mode:=enduser -component:=xd_nb04 $(DLL_OPTIONS)  1>log\xd_nb04.out 2>&1  
        copy $@ debug\$(@F)                                                        1>>log\xd_nb04.out 2>>&1
        xstrip -q -n -t $@                                                         1>>log\xd_nb04.out 2>>&1
        @echo ====== XD_NB04.DLL ============ OK!
        @echo.

$(XD_NB09): $(DLLSTARTUP) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86)
        xc =p src\xd_prj\xd.prj -mode:=enduser -component:=xd_nb09 $(DLL_OPTIONS)  1>log\xd_nb09.out 2>&1  
        copy $@ debug\$(@F)                                                        1>>log\xd_nb09.out 2>>&1
        xstrip -q -n -t $@                                                         1>>log\xd_nb09.out 2>>&1
        @echo ====== XD_NB09.DLL ============ OK!
        @echo.

$(XD_EDIF): $(DLLSTARTUP) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86)
        xc =p src\xd_prj\xd.prj -mode:=enduser -component:=xd_edif $(DLL_OPTIONS)  1>log\xd_edif.out 2>&1  
        copy $@ debug\$(@F)                                                        1>>log\xd_edif.out 2>>&1
        xstrip -q -n -t $@                                                         1>>log\xd_edif.out 2>>&1
        @echo ====== XD_EDIF.DLL ============ OK!
        @echo.

$(XD): $(EXESTARTUP) $(STIMPLIB) $(XD_UTL) $(XD_DITLS) $(XD_NB04) $(XD_NB09) $(XD_EDIF) $(WINSYM_X86)
        xc =p =a src\xd_prj\xd.prj -mode:=enduser -component:=xd -edition:=professional $(DLL_OPTIONS) 1>log\xd.out 2>&1  
        copy $@ debug\$(@F)                                                                            1>>log\xd.out 2>>&1
        xstrip -q -n -t $@                                                                             1>>log\xd.out 2>>&1
        @echo ====== XD.EXE ============ OK!
        @echo.

$(XD_SRV): $(XD)
        xc =p src\xd_prj\xd_srv.prj -mode:=enduser $(DLL_OPTIONS)  1>log\xd_srv.out 2>&1  
        copy $@ debug\$(@F)                                        1>>log\xd_srv.out 2>>&1
        xstrip -q -n -t $@                                         1>>log\xd_srv.out 2>>&1
        @echo ====== XD_SRV.EXE ============ OK!
        @echo.

$(XD_DEMON): $(XD)
        xc =p src\xd_prj\xd_demon.prj -mode:=enduser $(DLL_OPTIONS)  1>log\xd_demon.out 2>&1  
        copy $@ debug\$(@F)                                          1>>log\xd_demon.out 2>>&1
        xstrip -q -n -t $@                                           1>>log\xd_demon.out 2>>&1
        @echo ====== XD_DEMON.EXE ============ OK!
        @echo.

$(XD_TCP): $(DLLSTARTUP) $(STIMPLIB) $(WINSYM_X86)
        xc =p src\xd_prj\xd_trans.prj -mode:=enduser -transport:=tcp $(DLL_OPTIONS)  1>log\xd_tcp.out 2>&1  
        copy $@ debug\$(@F)                                                          1>>log\xd_tcp.out 2>>&1
        xstrip -q -n -t $@                                                           1>>log\xd_tcp.out 2>>&1
        @echo ====== XD_T_TCP.DLL ============ OK!
        @echo.

$(HIS): $(EXESTARTUP) $(STIMPLIB) $(XD_UTL) $(XD_DITLS)
        xc src\xd_prj\his =p -mode:=enduser $(DLL_OPTIONS)  1>log\his.out 2>&1  
        copy $@ debug\$(@F)                                 1>>log\his.out 2>>&1
        xstrip -q -n -t $@                                  1>>log\his.out 2>>&1
        @echo ====== HIS.EXE =========== OK!
        @echo.


#=============================================================================
# Build XProf Tools
#=============================================================================
$(XPROFAPI): $(DLLSTARTUP) $(STIMPLIB) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86)
        xc src\xd_prj\xpdump =p -mode:=enduser -component:=xprofapi $(DLL_OPTIONS)  1>log\xprofapi.out 2>&1  
        copy $@ debug\$(@F)                                                         1>>log\xprofapi.out 2>>&1
        xstrip -q -n -t $@                                                          1>>log\xprofapi.out 2>>&1
        @echo ====== XPROFAPI.DLL =========== OK!
        @echo.

$(XPDUMP): $(EXESTARTUP) $(XPROFAPI) 
        xc src\xd_prj\xpdump =p -mode:=enduser -component:=xpdump $(DLL_OPTIONS)  1>log\xpdump.out 2>&1  
        copy $@ debug\$(@F)                                                       1>>log\xpdump.out 2>>&1
        xstrip -q -n -t $@                                                        1>>log\xpdump.out 2>>&1
        @echo ====== XPDUMP.EXE =========== OK!
        @echo.

$(XPROF): $(EXESTARTUP) $(STLIB) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86) $(XPROFAPI)
        xc src\xd_prj\xprof =p -mode:=enduser $(DLL_OPTIONS)  1>log\xprof.out 2>&1   
        copy $@ debug\$(@F)                                   1>>log\xprof.out 2>>&1
        xstrip -q -n -t $@                                    1>>log\xprof.out 2>>&1
        @echo ====== XPROF.EXE =========== OK!
        @echo.

$(XPROFMEM): $(EXESTARTUP) $(STLIB) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86) $(XPROFAPI)
        xc src\xd_prj\xprofmem =p -mode:=enduser $(DLL_OPTIONS)  1>log\xprofmem.out 2>&1  
        copy $@ debug\$(@F)                                      1>>log\xprofmem.out 2>>&1
        xstrip -q -n -t $@                                       1>>log\xprofmem.out 2>>&1
        @echo ====== XPROFMEM.EXE =========== OK!
        @echo.


#=============================================================================
# Build General Tools
#=============================================================================
$(XSTRIP): $(EXESTARTUP) $(STLIB) $(XD_UTL) $(XD_DITLS) $(WINSYM_X86)
        xc src\xd_prj\xstrip =p -mode:=enduser $(DLL_OPTIONS)  1>log\xstrip.out 2>&1  
        copy $@ debug\$(@F)                                    1>>log\xstrip.out 2>>&1
        xstrip -q -n -t $@                                     1>>log\xstrip.out 2>>&1
        @echo ====== XSTRIP.EXE =========== OK!
        @echo.

$(XDASM): $(EXESTARTUP) $(XD)
        xc src\xdasm\xdasm =p =a -mode:=enduser $(DLL_OPTIONS)  1>log\xdasm.out 2>&1  
        copy $@ debug\$(@F)                                     1>>log\xdasm.out 2>>&1
        xstrip -q -n -t $@                                      1>>log\xdasm.out 2>>&1
        @echo ====== XDASM.EXE ========== OK!
        @echo.

$(XLIB): $(EXESTARTUP) $(STLIB)
        xc src\xlib\xlib =p -mode:=enduser $(LIB_OPTIONS)  1>log\xlib.out 2>&1  
        copy $@ debug\$(@F)                                1>>log\xlib.out 2>>&1
        xstrip -q -n -t $@                                 1>>log\xlib.out 2>>&1
        @echo ====== XLIB.EXE ========== OK!
        @echo.

$(H2D): $(EXESTARTUP) $(STLIB)
        -@del /q enduser\tmp
        xc src\h2d\h2d =p -mode:=enduser $(LIB_OPTIONS)  1>log\h2d.out 2>&1  
        copy $@ debug\$(@F)                              1>>log\h2d.out 2>>&1
        xstrip -q -n -t $@                               1>>log\h2d.out 2>>&1
        @echo ====== H2D.EXE =========== OK!
        @echo.

$(XPVIEW): 
        cd src\xpview\Win32
        nmake /f xpview.mak CFG="xpview - Win32 Release"     1>..\..\..\log\xpview.out 2>&1
        cd ..\..\..
        copy src\xpview\Win32\Release\xpview.exe $(XPVIEW)   1>>log\xpview.out 2>>&1
        @echo ====== $@ ====== OK!
        @echo.

$(XRC):
        cd src\xrc
        nmake /f xrc.mak CFG="xrc - Win32 Release"    1>..\..\log\xrc.out 2>&1
        cd ..\..
        copy src\xrc\Release\xrc.exe $(XRC)           1>>log\xrc.out 2>>&1
        @echo ====== $@ ====== OK!
        @echo.

$(XLINK):
        -@del /q enduser\tmp
        cd src\xlink
#        ..\..\msvc9.cmd nmake -f xlink.mkf.msvc "MODE=$(MODE)"  1>..\..\log\xlink.out 2>&1
        nmake -f xlink.mkf.msvc "MODE=$(MODE)"  1>..\..\log\xlink.out 2>&1
        cd ..\..
        copy src\xlink\xlink.exe $(XLINK)               1>>log\xlink.out 2>>&1
        @echo ====== XLINK.EXE =========== OK!
        @echo.


#=============================================================================
# Build XDS IDE
#=============================================================================
$(XDS):
        copy src\xShell\src\noexpire.h src\xShell\src\expire.h   1>log\xdside.out 2>&1
        cd src\xShell
#        ..\..\msvc9.cmd nmake /f xds.mak ALL CFG="xds - Win32 Release"         1>>..\..\log\xdside.out 2>>&1
        nmake /f xds.mak ALL CFG="xds - Win32 Release"         1>>..\..\log\xdside.out 2>>&1
        cd ..\..
        copy src\xShell\Release\xds.exe       $(XDS)           1>>log\xdside.out 2>>&1
        copy src\xShell\Release\res_mod.dll   $(XDSRES)        1>>log\xdside.out 2>>&1
        copy src\xShell\Release\c_lang.dll    $(CLANG)         1>>log\xdside.out 2>>&1
        copy src\xShell\Release\java_lang.dll $(JAVALANG)      1>>log\xdside.out 2>>&1
        copy src\xShell\Release\m2_lang.dll   $(M2LANG)        1>>log\xdside.out 2>>&1
        copy enduser\xds\bin\xdsx86.ini       enduser\xds\bin\xds.ini  1>>log\xdside.out 2>>&1
        @echo ====== $@ ====== OK!
        @echo.


#=============================================================================
# Build documentation
#=============================================================================
$(CONVERTN): $(M)
        cd Doc\convert
        nmake -f convertn.mak    1>..\..\log\convertn.out 2>&1
        copy convertn.exe ..\    
        cd ..\..
        @echo ====== $@ ====== OK!
        @echo.

$(XDSENVHLP): $(M) $(CONVERTN)
        cd src\xshell\help
        ..\..\..\doc\convertn -rc2 xdsenv.tex         1>..\..\..\log\xdsenvhlp.out  2>&1
        hcrtf -xn xdsenv.hpj                          1>>..\..\..\log\xdsenvhlp.out 2>>&1
        cd ..\..\..
        copy src\xshell\help\xdsenv.hlp $(XDSENVHLP)  1>>log\xdsenvhlp.out 2>>&1
        copy src\xshell\help\xdsenv.cnt $(XDSENVCNT)  1>>log\xdsenvhlp.out 2>>&1
        @echo ====== XDSENV.HLP =========== OK!
        @echo.

$(H2DHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo. >doc\frontend.tex
        echo. >>doc\backend.tex
        cd doc\h2d
        nmake /f h2d.mkf FORMAT=hlp  1>..\..\log\h2dhlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(O2REPHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo. >doc\frontend.tex
        echo. >>doc\backend.tex
        cd doc\o2rep
        nmake /f o2rep.mkf FORMAT=hlp  1>..\..\log\o2rephlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XCHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo \topspeedtrue >doc\frontend.tex
        echo \gencodetrue >doc\backend.tex
        echo \geninteltrue >>doc\backend.tex
        cd doc\xdsug
        -@del *.?lg *.rtf *.hlp *.cnt
        nmake /f xc.mkf FORMAT=hlp          1>..\..\log\xchlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XDHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo \gencodetrue >doc\backend.tex
        echo \geninteltrue >>doc\backend.tex
        cd doc\xd
        nmake /f xd.mkf FORMAT=hlp  1>..\..\log\xdhlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XDSLIBHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo. >doc\frontend.tex
        echo. >>doc\backend.tex
        cd doc\xdslib
        nmake /f xdslib.mkf FORMAT=hlp  1>..\..\log\xdslibhlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XMHLP): $(M) $(CONVERTN)
        echo \winnttrue >doc\platform.tex
        echo \topspeedtrue >doc\frontend.tex
        echo \genctrue >doc\backend.tex
        cd doc\xdsug
        nmake /f xm.mkf FORMAT=hlp 1>..\..\log\xmhlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XDSHLP): $(M) $(CONVERTN)
        cd doc\xds
        mkhlp  1>..\..\log\xdshlp.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.


$(H2DPDF): $(M)
        cd doc\h2d
        nmake -f h2d.mkf format=pdf  1>..\..\log\h2dpdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(O2REPPDF): $(M)
        cd doc\o2rep
        nmake -f o2rep.mkf format=pdf  1>..\..\log\o2reppdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XCPDF): $(M)
        echo \winnttrue >doc\platform.tex
        echo \topspeedtrue >doc\frontend.tex
        echo \gencodetrue >doc\backend.tex
        echo \geninteltrue >>doc\backend.tex
        cd doc\xdsug
        -@del *.dvi *.?lg *.pdf *.aux *.log *.toc *.ind *.idx *.lot *.ext
        nmake -f xc.mkf format=pdf  1>..\..\log\xcpdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XDPDF): $(M)
        echo \winnttrue >doc\platform.tex
        echo \gencodetrue >doc\backend.tex
        echo \geninteltrue >>doc\backend.tex
        cd doc\xd
        nmake -f xd.mkf format=pdf  1>..\..\log\xdpdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XDSLIBPDF): $(M)
        cd doc\xdslib
        nmake -f xdslib.mkf format=pdf  1>..\..\log\xdslibpdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.

$(XMPDF): $(M)
        echo \winnttrue >doc\platform.tex
        echo \topspeedtrue >doc\frontend.tex
        echo \genctrue >doc\backend.tex
        cd doc\xdsug
        nmake -f xm.mkf format=pdf  1>..\..\log\xmpdf.out 2>&1
        cd ..\..
        @echo ====== $@ =========== OK!
        @echo.


#=============================================================================
# Cleanup 
#=============================================================================
# NOTE: Help generation disabled 
#cleanup: cleanbin cleandoc
cleanup: cleanbin 
        -@del /q .\log\*        1>nul 2>nul
        -@del /q .\*.tmp        1>nul 2>nul
        @echo.
        @echo ====== Cleanup is completed ======
        @echo.

cleanbin: cleanxds cleanc
        @del /q debug           1>nul 2>nul
        @echo ====== Cleanup of binary files is completed ======
        @echo.

cleanxds:
        -@del $(LIBXDS)          $(LIBTS)
        -@del $(MSVCMKF)         $(WATCOMMKF)    $(UNIXMKF)      $(CLANGMKF)
        -@del $(MSVCTSMKF)       $(WATCOMTSMKF)  $(UNIXTSMKF)    $(CLANGTSMKF)
        -@del $(XM)              
        -@del $(RMSYM)
        -@del $(MERGE)
        -@del $(REEXPORT)
        -@del $(WINDEF)
        -@del $(IMPORT32)
        -@del $(EXESTARTUP)      $(DLLSTARTUP)   $(CSTARTUP)     $(XSTARTUP)
        -@del $(STLIB)           $(MTLIB)              
        -@del $(STDLL)           $(MTDLL)        
        -@del $(STIMPLIB)        $(MTIMPLIB)     
        -@del $(TSSTLIB)         $(TSMTLIB)      
        -@del $(TSSTDLL)         $(TSMTDLL)
        -@del $(TSSTIMPLIB)      $(TSMTIMPLIB)
        -@del $(VTERMLIB)        
        -@del $(XC)              
        -@del $(XD_UTL)          $(XD_DITLS)     
        -@del $(XD_NB04)         $(XD_NB09)      
        -@del $(XD_EDIF)         
        -@del $(XD)                           
        -@del $(XD_SRV)          $(XD_TCP)
        -@del $(XD_DEMON)
        -@del $(HIS)                             
        -@del $(XPROF)           $(XPROFMEM)                     
        -@del $(XPROFAPI)
        -@del $(XPDUMP)                  
        -@del $(XSTRIP)                  
        -@del $(XDASM)                   
        -@del $(XLIB)                            
        -@del $(H2D)
        -@del $(ARCHIVE)                       
        -@del $(ARCHIVE_DBG)                       
        -@del /q enduser\tmp                  1>nul 2>nul
        -@del enduser\xds\def\ob2\*.odf       1>nul 2>nul
        -@del /q enduser\xds\C\C\*            1>nul 2>nul
        -@del /q enduser\xds\include\C\*      1>nul 2>nul
        -@del /q enduser\xds\sym\c            1>nul 2>nul
        -@del /q enduser\xds\sym\x86          1>nul 2>nul
        -@del /q enduser\xds\bin\xds*.dll     1>nul 2>nul
        -@del *.lib                           1>nul 2>nul
        -@del *.eul                           1>nul 2>nul
        @echo ====== Cleanup of xds generated files is completed ======
        @echo.

cleanc:
# NOTE: IDE generation disabled
#        -@del $(XDS)          $(XDSRES)
#        -@del $(CLANG)
#        -@del $(JAVALANG)
#        -@del $(M2LANG)
        -@del $(XPVIEW)
        -@del $(XRC)
        -@del $(XLINK)                   
# NOTE: Help generation disabled 
#        -@del $(XDSENVHLP)    $(XDSENVCNT)
        -@del /q src\xrc\Release           1>nul 2>nul
        -@del /q src\xShell\Release        1>nul 2>nul
        -@del /q src\xShell\Edu            1>nul 2>nul
        -@del /q src\xShell\obj            1>nul 2>nul
        -@del /q src\xShell\objedu         1>nul 2>nul
        @echo ====== Cleanup of other files is completed ======
        @echo.

cleandoc:
        -@del $(CONVERTN)                                      1>nul 2>nul
        -@@rmdir /Q /S Doc/convert/Debug                       1>nul 2>nul
        -@del /Q /S Doc/convert/*.exe                          1>nul 2>nul
        -@del $(H2DHLP)          $(H2DCNT)       $(H2DPDF)     1>nul 2>nul
        -@del $(O2REPHLP)        $(O2REPCNT)     $(O2REPPDF)   1>nul 2>nul
        -@del $(XCHLP)           $(XCCNT)        $(XCPDF)      1>nul 2>nul
        -@del $(XDHLP)           $(XDCNT)        $(XDPDF)      1>nul 2>nul
        -@del $(XDSLIBHLP)       $(XDSLIBCNT)    $(XDSLIBPDF)  1>nul 2>nul
        -@del $(XMHLP)           $(XMCNT)        $(XMPDF)      1>nul 2>nul
        -@del $(XDSHLP)
        cd doc
        -cleanup                                               1>nul 2>nul
        cd ..
        @echo ====== Cleanup of documentation is completed ======
        @echo.

