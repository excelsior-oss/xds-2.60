@echo off
set EUDIR=..\..\enduser\xds


for %%i in (cnt,hlg,hlp,rlg,rtf)             do  if exist *.%%i del /F /Q *.%%i
for %%i in (idx,ind,ilg,pdf,log,toc,out,aux) do  if exist *.%%i del /F /Q *.%%i
for %%i in (html,chm,hha,hhc,clg)            do  if exist *.%%i del /F /Q *.%%i

for %%i in (GID) do  if exist *.%%i del /F /Q *.%%i 1>nul 2>nul


for %%i in (cnt,hlp,pdf,chm) do  if exist ..\xd.%%i del /F /Q ..\xd.%%i

for %%i in (cnt,hlp) do  if exist %EUDIR%\bin\xd.%%i del /F /Q %EUDIR%\bin\xd.%%i
for %%i in (chm)     do  if exist %EUDIR%\doc\xd.%%i del /F /Q %EUDIR%\doc\xd.%%i
for %%i in (pdf)     do  if exist %EUDIR%\pdf\xd.%%i del /F /Q %EUDIR%\pdf\xd.%%i
