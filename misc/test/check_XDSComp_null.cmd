del /q sym\*.*
del errinfo.$$$
bin\xc =p src/m/xc.prj -decor=rt -target:=NULL
if exist errinfo.$$$ %XDSDIR%\bin\his
