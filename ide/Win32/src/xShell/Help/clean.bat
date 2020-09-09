@echo off

for %%i in (cnt,hlg,hlp,rlg,rtf)             do  if exist *.%%i del /F /Q *.%%i
for %%i in (idx,ind,ilg,pdf,log,toc,out,aux) do  if exist *.%%i del /F /Q *.%%i
for %%i in (html,chm,hha,hhc)                do  if exist *.%%i del /F /Q *.%%i

for %%i in (GID) do  if exist *.%%i del /F /Q *.%%i 1>nul 2>nul


for %%i in (cnt,hlp,pdf,chm) do  if exist xdsenv.%%i del /F /Q xdsenv.%%i

