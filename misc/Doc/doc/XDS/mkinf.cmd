..\converto -i xds.tex
if errorlevel 1 goto quit
ipfc /inf /l:ENU /d:1 /c:850 xds.ipf
if errorlevel 1 goto quit
copy xds.inf ..\..\enduser\xds\doc\xds.inf

:quit