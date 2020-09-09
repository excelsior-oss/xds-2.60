del his.exe      
del xprof.exe    
del xpdump.exe   
del xprofapi.dll 
del xstrip.exe   

xc =p his.prj      -decor="rtp"
xc =p xpdump.prj   -decor="rtp" -component:=xprofapi
xc =p xpdump.prj   -decor="rtp" -component:=xpdump
xc =p xprof.prj    -decor="rtp"
xc =p xprofmem.prj -decor="rtp"
xc =p xstrip.prj   -decor="rtp"


