call %~dp0\..\.config\setenv.bat

xc =p src\xd_prj\his.prj    -decor="rtp" -mode:=enduser
xc =p src\xd_prj\xpdump.prj -decor="rtp" -mode:=enduser -component:=xprofapi
xc =p src\xd_prj\xpdump.prj -decor="rtp" -mode:=enduser -component:=xpdump
xc =p src\xd_prj\xprof.prj  -decor="rtp" -mode:=enduser
xc =p src\xd_prj\xstrip.prj -decor="rtp" -mode:=enduser

