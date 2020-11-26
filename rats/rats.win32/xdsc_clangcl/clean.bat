@echo off
set CUR_DIR=%~dp0

set RAM_DISK=B:
set RAM_TMPDIR=TEMP_RATS_XDS_260

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
call :lbl_set_parent_dir "%CD%"

if exist "%RAM_DISK%\%RAM_TMPDIR%\%PARENT_DIR%"  goto :EOF

set RAM_RATS_TEMP_DIR="%RAM_DISK%\%RAM_TMPDIR%"
if not exist "%RAM_RATS_TEMP_DIR%"  mkdir "%RAM_RATS_TEMP_DIR%"

set RAM_RATS_TEMP_DIR="%RAM_RATS_TEMP_DIR%\%PARENT_DIR%"
if not exist "%RAM_RATS_TEMP_DIR%"  mkdir "%RAM_RATS_TEMP_DIR%"

mklink /D /J "%CUR_DIR%tmp" "%RAM_RATS_TEMP_DIR%"
goto :EOF

:lbl_set_parent_dir
set PARENT_DIR=%~n1
goto :EOF

