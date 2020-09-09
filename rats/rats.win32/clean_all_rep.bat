@echo off
if exist test_all.log del test_all.log

echo xdsn is cleared > test_all.log
cd xdsn
call clean_rep.bat
cd ..

echo xdsc is cleared >> test_all.log
cd xdsc 
call clean_rep.bat
cd ..

echo tscpn is cleared >> test_all.log
cd tscpn
call clean_rep.bat
cd ..

echo tscpc is cleared >> test_all.log
cd tscpc
call clean_rep.bat
cd ..

:quite

echo All rep-files are cleared >> test_all.log
