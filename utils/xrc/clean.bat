@echo off

for %%i in (log,ch,sym,src\Debug) do if exist %%i rmdir /Q /S %%i

for %%i in (mkf,$$$,lnk,out,exe,ilk,pdb)  do  if exist *.%%i  del /Q *.%%i

for %%i in (MSG00001.bin,XRCMSG.h,XRCMSG.rc) do  if exist src\*.%%i  del /Q src\*.%%i
