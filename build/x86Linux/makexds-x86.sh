#!/bin/bash

. ./setenv.sh

Check () {
  if test $? -gt 0 ; then echo;echo "*** SOME ERROR HAPPENED ***";echo; exit 1; fi
}

DO () {
  echo
  echo PWD=$PWD
  echo
  echo $*
  echo 
        /bin/sh -c "$*"
  Check
}

init() {
  mkdir -p enduser/xds/lib
  mkdir -p enduser/xds/lib/x86
  mkdir -p enduser/xds/include
  mkdir -p enduser/xds/include/x86
  mkdir -p enduser/xds/sym
  mkdir -p enduser/xds/sym/x86
  mkdir -p enduser/xds/C
  mkdir -p enduser/xds/C/x86
  mkdir -p enduser/xds/bin
  mkdir -p enduser/tmp
  mkdir -p enduser/xds/samples/X11

  ln -sf $PWD/src/isodef/*.def enduser/xds/def/iso
  ln -sf $PWD/src/pimlib/*.def enduser/xds/def/pim
  ln -sf $PWD/src/syslib/*.def enduser/xds/def/iso
  ln -sf $PWD/src/ulib/*.def   enduser/xds/def/xds
  rm -f  enduser/xds/def/xds/dllRTS.def enduser/xds/def/xds/xShell.def
  ln -sf $PWD/src/POSIX/*.def $PWD/src/POSIX/posix.prj enduser/xds/def/POSIX
  ln -sf $PWD/src/X11/def/X11/* enduser/xds/def/X11
  ln -sf $PWD/src/X11/def/Xm/* enduser/xds/def/Xm
  ln -sf $PWD/src/X11/X*.prj enduser/xds
  ln -sf $PWD/src/X11/demos/*.mod enduser/xds/samples/X11
  ln -sf $PWD/src/X11/XmDemos.ad enduser/xds/samples/X11

  ln -sf $PWD/src/os/POSIX/*.c enduser/xds/C/x86
  ln -sf $PWD/src/xr/C/*.c enduser/xds/C/x86
  ln -sf $PWD/src/xr/C-Native/*.c enduser/xds/C/x86
  ln -sf $PWD/src/xr/C-Native/x86Linux/*.c enduser/xds/C/x86
  ln -sf $PWD/src/xr/C-Native/x86Linux/*.s enduser/xds/C/x86

  ln -sf $PWD/src/xr/C/*.h enduser/xds/include/x86
  ln -sf $PWD/src/os/POSIX/*.h enduser/xds/include/x86

  chmod 755 enduser/xds/bin/xdssamp enduser/xds/bin/xcwork enduser/xds/apisyms-x86 enduser/xds/samples/buildx86
}


libxds_x86(){
  rm enduser/tmp/*
  DO xm =p src/m/lib -mode:=nodebug -native_library:+ $OPTIONS 
#  DO xm =p src/m/lib -mode:=work -placement:=release -native_library:+ $OPTIONS
  cp enduser/tmp/*.sym enduser/xds/sym/x86
}

libmkf_x86(){
  pushd enduser/xds/lib/x86
  DO xm =g ../../../../src/m/lib -mode:=user -native_library:+
#  DO xm =g ../../../../src/m/lib -mode:=debug -native_library:+
  popd
}


libts_x86(){
  rm enduser/tmp/*
  ln -fs $PWD/src/TSlibs/POSIX/*.c enduser/xds/C/x86
  rm enduser/xds/C/BiosIO.c  
  ln -fs $PWD/src/TSlibs/Linux/*.h enduser/xds/include/x86
  DO xm =p src/TSlibs/tslib -mode:=nodebug -native_library:+ $OPTIONS 

  DO xc =p src/POSIX/posix.prj -sym_place:=enduser/tmp:enduser/xds/sym/x86 $OPTIONS
  DO xc =p =a src/TSlibs/tslib -mode:=nodebug -native_library:+ -lookup=*.sym=enduser/tmp $OPTIONS 
#  DO xc =p =a src/TSlibs/tslib -mode:=debug -native_library:+ $OPTIONS
  DO make -f enduser/tmp/tslib.mkf
}


libtsmkf_x86(){
  cd enduser/xds/lib/x86
  DO xm =g ../../../../src/TSlibs/tslib -mode:=user -native_library:+ $OPTIONS
#  DO xm =g ../../../../src/TSlibs/tslib -mode:=debug -native_library:+ $OPTIONS
  cd ../../../..
}


xc_through_xm(){
  DO xm =a =p src/m/xc -mode:=enduser -ts_compatible:+ $OPTIONS -native_library:+ -usestaticclibs:+ -lookup=*.c=enduser/tmp -lookup=*.h=enduser/tmp -xdsdir=enduser/xds
#  DO xm =a =p src/m/xc -mode:=work -placement:=release $OPTIONS -native_library:+ -lookup=*.c=enduser/tmp -lookup=*.h=enduser/tmp -xdsdir=enduser/xds
#  DO xm =p src/m/xc -mode:=work -placement:=release $OPTIONS
}


xc_through_xc(){
  rm enduser/tmp/*
  DO xc =p src/m/xc -mode:=enduser -ts_compatible:+ $OPTIONS -native_library:+ -usestaticclibs:+
#  DO xc =p src/m/xc -mode:=work -placement:=release $OPTIONS
}


xlib_x86(){
  rm enduser/tmp/*
  DO enduser/xds/bin/xc =p =a src/xlib/xlib.prj -mode:=enduser
  mv xlib enduser/xds/bin
}


xlink_x86(){
  pushd src/xlink
  DO make -f xlink.mkf.linux
  popd
  mv src/xlink/xlink enduser/xds/bin
}


h2d(){
  rm enduser/tmp/*
  DO enduser/xds/bin/xc =p src/h2d/h2d -mode:=enduser $OPTIONS -usestaticclibs:+
}

posix_x86(){
  rm enduser/tmp/*
  DO xc =p src/POSIX/posix.prj -sym_place:=$SYM_PLACE_X86 $OPTIONS
  DO xc =p src/POSIX/mac/libmac_posix.prj -lib_place:=$LIB_PLACE_X86 -sym_place:=$SYM_PLACE_X86 -tmp_place:=$TMP_PLACE_X86 $OPTIONS
  cp enduser/tmp/*.sym enduser/xds/sym/x86
}

linux_x86(){
  rm enduser/tmp/*
  DO xc =p src/Linux/linux.prj -sym_place:=$SYM_PLACE_X86 $OPTIONS
  cp enduser/tmp/*.sym enduser/xds/sym/x86
}


x11_x86(){
  rm -f enduser/tmp/*
  DO  xc =p src/X11/X11.prj -sym_place:=$SYM_PLACE_X86 $OPTIONS
  DO  xc =p src/X11/macro/X11/libmac_X11.prj -lib_place:=$LIB_PLACE_X86 -sym_place:=$SYM_PLACE_X86 -tmp_place:=$TMP_PLACE_X86 $OPTIONS 
 
  DO  xc =p src/X11/Xm.prj -sym_place:=$SYM_PLACE_X86 $OPTIONS
  DO  xc =p src/X11/XmP.prj -sym_place:=$SYM_PLACE_X86 $OPTIONS
  DO  xc =p src/X11/macro/Xm/libmac_Xm.prj -lib_place:=$LIB_PLACE_X86 -sym_place:=$SYM_PLACE_X86 -tmp_place:=$TMP_PLACE_X86 $OPTIONS

  cp enduser/tmp/*.sym enduser/xds/sym/x86
}

arch(){
  VERSION_CONTAINER=src/u/xm.ob2
  VERSION=`grep "vers =" $VERSION_CONTAINER | sed "s/[^0123456789]//g"`
  DISTRIBUTION_NAME=`echo xds-x86-$VERSION-$TYPE-linux | awk '{ print tolower($1);}'`
#  DISTRIBUTION_NAME_P=`echo xds-x86-$VERSION-$TYPE-linux | awk '{ print tolower($1);}'`
  cd enduser
  DO "tar -hcf - --exclude xds/licenses/xds.txt xds | gzip >../$DISTRIBUTION_NAME.tgz"
#  DO "tar -hcf - --exclude xds/licenses/xds.txt --exclude xds/pdf xds | gzip >../$DISTRIBUTION_NAME_P.tgz"
  cd ..
} 


clean() {
  rm -f enduser/tmp/*
  rm -f enduser/xds/bin/h2d 
  rm -f enduser/xds/bin/xc enduser/xds/lib/x86/* enduser/xds/sym/x86/* 
  rm -f enduser/xds/C/x86/* enduser/xds/include/x86/*
  rm -f src/xlink/*.o src/xlink/xlink

#  rm -f enduser
}

build_x86() {
  init
#  xlink_x86
  libxds_x86
  libmkf_x86
#  xc_through_xm
  xc_through_xc
#  xlib_x86
  posix_x86
  linux_x86
  x11_x86
  libts_x86  
  libtsmkf_x86
}

all() {
  build_x86
  h2d
#  arch
}

half_x86() {
  init
  libxds_x86
  xc_through_xm
  libts_x86  
  libtsmkf_x86
#  arch
}



trial() {
  OPTIONS="-decor=rt -woff+ -trialversion:+ -gentrialsym:+ -new_name_separator:=_"
  TYPE=trial
}

OPTIONS="-decor=rt -woff+"
TYPE=enduser
SYM_PLACE_X86=enduser/tmp:enduser/xds/sym/x86
TMP_PLACE_X86=enduser/tmp
LIB_PLACE_X86=enduser/xds/lib/x86


for par in $*; do
  $par
done


echo  
echo ------
echo  DONE
echo ------
echo
