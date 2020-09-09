@echo off
call %~dp0\..\..\..\..\.config\setenv.bat

rem --- set path to the convertn.exe
set CONVERTN_DIR=%~dp0..
set PATH=%CONVERTN_DIR%;%PATH%

call clean.bat

echo Build Native XDS-x86 User's Guide
echo \winnttrue    >..\platform.tex
echo \topspeedtrue >..\frontend.tex
echo \gencodetrue  >..\backend.tex
echo \geninteltrue >>..\backend.tex
call mkf.bat xc pdf
call mkf.bat xc hlp 
call mkf.bat xc chm 

echo Build XDS-C User's Guide
echo \winnttrue    >..\platform.tex
echo \topspeedtrue >..\frontend.tex
echo \genctrue     >..\backend.tex
call mkf.bat xm pdf
call mkf.bat xm hlp 
call mkf.bat xm chm 
