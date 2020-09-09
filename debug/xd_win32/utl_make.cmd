call %~dp0\..\..\.config\setenv.bat

del his.exe      
del xprof.exe    
del xpdump.exe   
del xprofapi.dll 
del xstrip.exe   

xc =p ..\src\xd_prj\his.prj      -decor="rtp"
xc =p ..\src\xd_prj\xpdump.prj   -decor="rtp" -component:=xprofapi
xc =p ..\src\xd_prj\xpdump.prj   -decor="rtp" -component:=xpdump
xc =p ..\src\xd_prj\xprof.prj    -decor="rtp"
xc =p ..\src\xd_prj\xprofmem.prj -decor="rtp"
xc =p ..\src\xd_prj\xstrip.prj   -decor="rtp"


