call %~dp0\..\.config\setenv.bat

set MODE=work
set PLACEMENT=release

set XJRTS_OPTS=-JAVA_CALLCONV:=noee -edition:=professional

xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_utl
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_ditls
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_nb04
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_nb09
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_edif
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd_dwrf
xc =p src\xd_prj\xd.prj       -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -component:=xd %XJRTS_OPTS%

xc =p src\xd_prj\xd_srv.prj   -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT%
xc =p src\xd_prj\xd_trans.prj -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT% -transport:=tcp

xc =p src\xd_prj\hdrstrip.prj -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT%
xc =p src\xd_prj\xload.prj    -decor="rtp" -mode:=%MODE% -placement:=%PLACEMENT%
