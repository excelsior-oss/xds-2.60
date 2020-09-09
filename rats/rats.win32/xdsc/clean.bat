@echo off
set CUR_DIR=%~dp0

set RAM_DISK=B:
set RAM_TMPDIR=xds_260_tmp

rmdir /Q /S tmp 1>nul 2>nul
for %%i in (mod,def,ob2)  do  if exist tmp\%%i\*.* del /Q tmp\%%i\*.* 

if exist %RAM_DISK%  call :use_ram_disk 

if not exist tmp  mkdir tmp

mkdir tmp\def
mkdir tmp\mod
mkdir tmp\ob2
mkdir tmp\ref
mkdir tmp\sym
mkdir tmp\ch
mkdir tmp\obj
mkdir tmp\exe
mkdir tmp\log
mkdir tmp\tmp

del *.log *.mkf oct.ses comp.log comp1.log 1>nul 2>nul
for %%i in (prg,map,err,log)  do  if exist *.%%i del /Q *.%%i 

goto :EOF


:use_ram_disk

if not exist %RAM_DISK%\RATS                mkdir %RAM_DISK%\RATS
if not exist %RAM_DISK%\RATS\%RAM_TMPDIR%   mkdir %RAM_DISK%\RATS\%RAM_TMPDIR%
rdutil.exe link %CUR_DIR%tmp RATS\%RAM_TMPDIR%  

goto :EOF
