@set target=
@if not "%~1" == "" @set target=%~1\bin

@if exist "%target%" @goto ready
@echo Error: no target specified or invalid!
@echo Usage: update.cmd target_dir
@exit

:ready
@set source=enduser\xds\bin

@copy %source%\xd_ditls.dll %target%
@copy %source%\xd_nb04.dll  %target%
@copy %source%\xd_nb09.dll  %target%
@copy %source%\xd_utl.dll   %target%
@copy %source%\xd.exe       %target%
@copy %source%\xd.msg       %target%

@copy %source%\his.exe      %target%
@copy %source%\xprof.exe    %target%
@copy %source%\xpdump.exe   %target%
@copy %source%\xprofapi.dll %target%
@copy %source%\xstrip.exe   %target%

@copy %source%\xd_srv.exe   %target%
@copy %source%\xd_t_tcp.dll %target%

@set source=
@set target=

