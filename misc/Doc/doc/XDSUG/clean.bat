@echo off
set EUDIR=..\..\enduser\xds


for %%i in (cnt,hlg,hlp,rlg,rtf)             do  if exist *.%%i del /F /Q *.%%i
for %%i in (idx,ind,ilg,pdf,log,toc,out,aux) do  if exist *.%%i del /F /Q *.%%i
for %%i in (html,chm,hha,hhc,clg)            do  if exist *.%%i del /F /Q *.%%i

for %%i in (GID) do  if exist *.%%i del /F /Q *.%%i 1>nul 2>nul


for %%i in (cnt,hlp,pdf,chm) do  if exist ..\xc.%%i del /F /Q ..\xc.%%i
for %%i in (cnt,hlp,pdf,chm) do  if exist ..\xm.%%i del /F /Q ..\xm.%%i

for %%i in (cnt,hlp) do  if exist %EUDIR%\bin\xc.%%i del /F /Q %EUDIR%\bin\xc.%%i
for %%i in (chm)     do  if exist %EUDIR%\doc\xc.%%i del /F /Q %EUDIR%\doc\xc.%%i
for %%i in (pdf)     do  if exist %EUDIR%\pdf\xc.%%i del /F /Q %EUDIR%\pdf\xc.%%i

for %%i in (cnt,hlp) do  if exist %EUDIR%\bin\xm.%%i del /F /Q %EUDIR%\bin\xm.%%i
for %%i in (chm)     do  if exist %EUDIR%\doc\xm.%%i del /F /Q %EUDIR%\doc\xm.%%i
for %%i in (pdf)     do  if exist %EUDIR%\pdf\xm.%%i del /F /Q %EUDIR%\pdf\xm.%%i
