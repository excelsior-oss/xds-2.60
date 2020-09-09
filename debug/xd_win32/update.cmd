@set dist=
@if not "%~1" == "" @set dist=%~1\bin

@if exist "%dist%" @goto ready
@echo Error: no target specified or invalid!
@echo Usage: update.cmd target_dir
@exit

:ready
@echo.              
@echo   *** Update to %dist% ***
@echo.
@copy xd_ditls.dll "%dist%" 
@copy xd_nb04.dll  "%dist%" 
@copy xd_edif.dll  "%dist%" 
@copy xd_nb09.dll  "%dist%" 
@copy xd_utl.dll   "%dist%" 
@copy xd.exe       "%dist%" 
@copy xd.msg       "%dist%" 
@copy his.exe      "%dist%" 
@copy xprof.exe    "%dist%" 
@copy xpdump.exe   "%dist%" 
@copy xprofapi.dll "%dist%" 
@copy xprofmem.exe "%dist%" 
@copy xstrip.exe   "%dist%" 
@copy xd_demon.exe "%dist%"
@copy xd_srv.exe   "%dist%" 
@copy xd_t_tcp.dll "%dist%" 
rem @if not "%dist%" == "%XDSDIR%\bin\" @copy *.edi "%dist%" 
rem @if not "%dist%" == "%XDSDIR%\bin\" @copy dbg\*.dbg "%dist%dbg"

@set dist=
