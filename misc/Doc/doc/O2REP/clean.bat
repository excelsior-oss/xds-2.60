@echo off
set EUDIR=..\..\enduser\xds


for %%i in (cnt,hlg,hlp,rlg,rtf)             do  if exist *.%%i del /F /Q *.%%i
for %%i in (idx,ind,ilg,pdf,log,toc,out,aux) do  if exist *.%%i del /F /Q *.%%i
for %%i in (html,chm,hha,hhc,clg)            do  if exist *.%%i del /F /Q *.%%i

for %%i in (GID) do  if exist *.%%i del /F /Q *.%%i 1>nul 2>nul


for %%i in (cnt,hlp,pdf,chm) do  if exist ..\o2rep.%%i del /F /Q ..\o2rep.%%i

for %%i in (cnt,hlp) do  if exist %EUDIR%\bin\o2rep.%%i del /F /Q %EUDIR%\bin\o2rep.%%i
for %%i in (chm)     do  if exist %EUDIR%\doc\o2rep.%%i del /F /Q %EUDIR%\doc\o2rep.%%i
for %%i in (pdf)     do  if exist %EUDIR%\pdf\o2rep.%%i del /F /Q %EUDIR%\pdf\o2rep.%%i
