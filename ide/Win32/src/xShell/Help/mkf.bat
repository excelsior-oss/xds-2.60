@echo off
set LATEX= pdflatex -c-style-errors -halt-on-error -interaction=batchmode  
set MKIDX= makeindex


set TARGET=xdsenv

if "%1" == ""     goto :label_hlp 
if "%1" == "hlp"  goto :label_hlp 
if "%1" == "pdf"  goto :label_pdf 
if "%1" == "chm"  goto :label_chm 

goto :invalid_args

:label_hlp
echo Building %TARGET% in hlp format
convertn -rc2 %TARGET%.tex
if errorlevel 1 goto make_error
hcrtf -xn %TARGET%.hpj
if errorlevel 1 goto make_error
echo ============ make %TARGET% ============ OK!
goto :EOF


:label_pdf
echo Building %TARGET% in pdf format
%LATEX%  %TARGET%.tex
if errorlevel 1 goto make_error
copy %TARGET%.idx %TARGET%.ids
%MKIDX% %TARGET%.idx
if errorlevel 1 goto make_error
%LATEX% %TARGET%.tex
if errorlevel 1 goto make_error
copy %TARGET%.ids %TARGET%.idx
del %TARGET%.ids
echo ============ make %TARGET% ============ OK!
goto :EOF


:label_chm
echo Building %TARGET% in chm format
convertn -H -hp -m2 -s %TARGET%.tex
if errorlevel 1 goto make_error
hhc %TARGET%.hhp
if errorlevel 0 goto make_error
echo ============ make %TARGET% ============ OK!
goto :EOF


:make_error
echo *********** make %TARGET% failed *********** Erorr(s)!
exit 1

:invalid_args
echo Specify the target format
echo   mfk.bat hlp  - build Windows Help
echo   mfk.bat pdf  - build Abode PDF
echo   mfk.bat chm  - build Microsoft Compiled HTML Help
exit 1
