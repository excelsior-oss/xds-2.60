@echo off

for %%i in (log,ch,src\Debug) do if exist %%i rmdir /Q /S %%i

for %%i in (mkf,$$$,lnk,out,exe,dll,ilk,pdb)  do  if exist *.%%i  del /Q *.%%i

for %%i in (bin,sym)  do  if exist %%i\*.*  del /Q %%i\*.*
