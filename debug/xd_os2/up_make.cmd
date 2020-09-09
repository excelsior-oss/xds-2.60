@if NOT "%1" == "" @set target=%1
@if "%1" == "" @set target=

@echo.                             > %target%update.cmd
@echo @if "%%1" == "" @goto msg   >> %target%update.cmd 
@echo @set dist=%%1\              >> %target%update.cmd
@echo @copy xd_ditls.dll %%dist%% >> %target%update.cmd
@echo @copy xd_nb04.dll  %%dist%% >> %target%update.cmd
@echo @copy xd_nb09.dll  %%dist%% >> %target%update.cmd
@echo @copy xd_utl.dll   %%dist%% >> %target%update.cmd
@echo @copy xd.exe       %%dist%% >> %target%update.cmd
@echo @copy xd.msg       %%dist%% >> %target%update.cmd
@echo @copy his.exe      %%dist%% >> %target%update.cmd
@echo @copy xprof.exe    %%dist%% >> %target%update.cmd
@echo @copy xpdump.exe   %%dist%% >> %target%update.cmd
@echo @copy xprofapi.dll %%dist%% >> %target%update.cmd
@echo @copy xstrip.exe   %%dist%% >> %target%update.cmd
@echo @copy xd_srv.exe   %%dist%% >> %target%update.cmd
@echo @copy xd_t_tcp.dll %%dist%% >> %target%update.cmd
@echo @set dist=                  >> %target%update.cmd
@echo @goto end                   >> %target%update.cmd
@echo @:msg                       >> %target%update.cmd
@echo @echo No target specified!  >> %target%update.cmd
@echo @echo update.cmd dest_dir   >> %target%update.cmd
@echo @:end                       >> %target%update.cmd

@set target=

