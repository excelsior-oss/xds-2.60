..\convertn -r xds.tex
if errorlevel 1 goto quit
hcrtf -xn xds.hpj
if errorlevel 1 goto quit
copy xds.hlp ..\..\enduser\xds\bin\xds.hlp
:quit