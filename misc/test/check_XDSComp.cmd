mkdir XDSComp
cd XDSComp
del /s /q *.*

\\igloo\vss\win32\ss Get $/XDSComp -R -GN -I- -GWR -W -Yguest     >>report
\\igloo\vss\win32\ss Get $/XDSMisc/test/XDSComp/ -R -GN -I- -GWR -W -Yguest

call ..\check_XDSComp_x86.cmd >report 2>errors

call ..\check_XDSComp_null.cmd >report 2>errors

cd ..
