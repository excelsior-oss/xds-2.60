@echo off
call %~dp0\..\..\..\..\.config\setenv.bat

rem --- set path to the convertn.exe
set CONVERTN_DIR=%~dp0..
set PATH=%CONVERTN_DIR%;%PATH%

call clean.bat

echo Building XD User's Guide ...
echo \winnttrue >..\platform.tex
echo \gencodetrue >..\backend.tex
echo \geninteltrue >>..\backend.tex
call mkf.bat pdf
call mkf.bat hlp 
call mkf.bat chm 
