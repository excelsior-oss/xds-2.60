@del xd_ditls.dll 
@del xd_nb04.dll  
@del xd_nb09.dll  
@del xd_utl.dll   
@del xd.exe       

@del xd_srv.exe   
@del xd_t_tcp.dll 

@if NOT "%1" == "" goto set_mode
@set mode="=p"
@goto work
:set_mode
@set mode="=i"
:work

xc %mode% xd.prj       -decor="rtp" -component:=xd_utl    +xdebug   
xc %mode% xd.prj       -decor="rtp" -component:=xd_ditls  +xdebug
xc %mode% xd.prj =a    -decor="rtp" -component:=xd_nb04   +xdebug
xc %mode% xd.prj       -decor="rtp" -component:=xd_nb09   +xdebug
xc %mode% xd.prj       -decor="rtp" -component:=xd_edif   +xdebug
rem xc %mode% xd.prj =a    -decor="rtp" -component:=xd_xhll   +xdebug
rem xc %mode% xd.prj       -decor="rtp" -component:=xd_dwrf   +xdebug
xc %mode% xd.prj       -decor="rtp" -component:=xd        +xdebug -edition:=professional
xc %mode% xd_demon.prj -decor="rtp"                       +xdebug
xc %mode% xd_srv.prj   -decor="rtp"                       +xdebug
xc %mode% xd_trans.prj -decor="rtp" -transport:=tcp       +xdebug
