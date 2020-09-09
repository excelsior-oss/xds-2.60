copy %XDSDIR%\bin\xlink.exe bin
copy %XDSDIR%\bin\xc.cfg bin
copy %XDSDIR%\bin\xc.msg bin
copy %XDSDIR%\bin\xc.tem bin
del errinfo.$$$
echo ==============FIRST COMPILATION=====================
del /q sym\*.*
%XDSDIR%\bin\xc =p src/m/xc.prj -decor=rt +xdebug
copy xc.exe bin\xc.exe
del /q xc.exe                 

echo ==============SECOND COMPILATION=====================
del /q sym\*.*                   
bin\xc =p src/m/xc.prj -decor=rt  +xdebug
copy xc.exe bin\xc.exe           
del /q xc.exe                    

echo ==============THIRD COMPILATION=====================
del /q sym\*.*                   
bin\xc =p src/m/xc.prj -decor=rt  +xdebug
fc bin\xc.exe xc.exe 
if exist errinfo.$$$ %XDSDIR%\bin\his  
