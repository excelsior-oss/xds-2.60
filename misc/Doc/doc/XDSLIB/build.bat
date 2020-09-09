@echo off
call %~dp0\..\..\..\..\.config\setenv.bat

rem --- set path to the convertn.exe
set CONVERTN_DIR=%~dp0..
set PATH=%CONVERTN_DIR%;%PATH%

call clean.bat

echo Building XDS Library Guide and Reference ...
echo \winnttrue >..\platform.tex
echo. >..\frontend.tex
echo. >>..\backend.tex
call mkf.bat pdf
call mkf.bat hlp 
call mkf.bat chm 
