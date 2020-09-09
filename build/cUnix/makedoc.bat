@echo off
set CURRENT_DIR=%~dp0

SET PATH=%CURRENT_DIR%doc;%PATH%;

call setenv.bat

pushd doc

pushd convert
nmake -f convertn.mak    1>..\..\log\convertn.out 2>&1
copy convertn.exe ..\    
popd


echo \linuxtrue > platform.tex

pushd xdsug
echo \genctrue > ..\backend.tex

echo \topspeedtrue > ..\frontend.tex
nmake -f xm.mkf FORMAT=html
if errorlevel 1 goto error
nmake -f xm.mkf FORMAT=pdf
if errorlevel 1 goto error


echo \gencodetrue \geninteltrue > ..\backend.tex

echo \topspeedtrue > ..\frontend.tex
nmake -f xc.mkf FORMAT=html
if errorlevel 1 goto error
nmake -f xc.mkf FORMAT=pdf
if errorlevel 1 goto error
popd



pushd isolib
nmake -f isolib.mkf FORMAT=html
if errorlevel 1 goto error
popd

pushd h2d
nmake -f h2d.mkf FORMAT=html
if errorlevel 1 goto error
popd

pushd o2rep
nmake -f o2rep.mkf FORMAT=html
if errorlevel 1 goto error
popd

pushd xds
nmake -f xds.mkf FORMAT=html
if errorlevel 1 goto error
popd

pushd xdslib
nmake -f xdslib.mkf FORMAT=html
if errorlevel 1 goto error
popd


pushd isolib
nmake -f isolib.mkf FORMAT=pdf
if errorlevel 1 goto error
popd

pushd h2d
nmake -f h2d.mkf FORMAT=pdf
if errorlevel 1 goto error
popd

pushd o2rep
nmake -f o2rep.mkf FORMAT=pdf
if errorlevel 1 goto error
popd

pushd xdslib
nmake -f xdslib.mkf FORMAT=pdf
if errorlevel 1 goto error
popd


popd

echo ==== OK ====
goto quit

:error
echo **** ERROR ****
pause

:quit
