@echo off

for %%i in (ch,include,sym,)  do  if exist %%i rmdir /Q /S %%i 
for %%i in (log,enduser,)     do  if exist %%i rmdir /Q /S %%i 

for %%i in (err.txt,log.txt)  do  if exist %%i del /Q %%i 
for %%i in (lib,dll,mkf,pdb)  do  if exist *.%%i del /Q *.%%i 
for %%i in (out,log,tmp,bak,$$$)  do  if exist *.%%i del /Q *.%%i 
