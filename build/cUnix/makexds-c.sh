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
  mkdir -p enduser/xds/lib/C
  mkdir -p enduser/xds/include
  mkdir -p enduser/xds/include/C
  mkdir -p enduser/xds/sym
  mkdir -p enduser/xds/sym/C
  mkdir -p enduser/xds/C
  mkdir -p enduser/xds/C/C
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
  ln -sf $PWD/src/X11/x11_include enduser/xds/include/C
  ln -sf $PWD/src/X11/xm_include enduser/xds/include/C
  ln -sf $PWD/src/X11/demos/*.mod enduser/xds/samples/X11
  ln -sf $PWD/src/X11/XmDemos.ad enduser/xds/samples/X11

  ln -sf $PWD/src/os/POSIX/*.c enduser/xds/C/C
  ln -sf $PWD/src/xr/C/*.c enduser/xds/C/C
  ln -sf $PWD/src/xr/C-Native/*.c enduser/xds/C/C
  ln -sf $PWD/src/xr/C-Native/x86Linux/*.c enduser/xds/C/C
  ln -sf $PWD/src/xr/C-Native/x86Linux/*.s enduser/xds/C/C

  ln -sf $PWD/src/xr/C/*.h enduser/xds/include/C
  ln -sf $PWD/src/os/POSIX/*.h enduser/xds/include/C

  ln -sf $PWD/src/os/POSIX/*.h enduser/xds/include/C
  ln -sf $PWD/src/TSlibs/Linux/*.h enduser/xds/include/C
  ln -sf $PWD/src/TSlibs/POSIX/*.c enduser/xds/C/C

  chmod 755 enduser/xds/bin/xdssamp enduser/xds/bin/xmwork enduser/xds/apisyms-c enduser/xds/samples/buildc
}


libxds_C(){
  rm -f enduser/tmp/*
  DO xm =a =p src/m/lib -mode:=nodebug -native_library:- $OPTIONS -env_host=linux -env_target=linux
#  DO xm =a =p src/m/lib -mode:=work -placement:=release -native_library:- $OPTIONS -env_host=linux -env_target=linux
#  DO xm =p src/m/lib -mode:=debug -native_library:- $OPTIONS
  if [ -e "enduser/tmp/*.sym" ]
  then
    cp enduser/tmp/*.sym enduser/xds/sym/C
  fi
}

libmkf_C(){
  cd enduser/xds/lib/C
  DO xm =g ../../../../src/m/lib -mode:=user -native_library:-
  cd ../../../..
}


libts_C(){
  rm -f enduser/tmp/*
  rm enduser/xds/C/C/BiosIO.c  
  ln -sf $PWD/src/TSlibs/POSIX/BiosIO.c enduser/xds/C/C
  DO xm =p src/TSlibs/tslib -mode:=nodebug  $OPTIONS -xdsdir=enduser/xds
  DO make -f enduser/xds/lib/C/tslib.mkf
}

libtsmkf_C(){
  cd enduser/xds/lib/C
  DO xm =g ../../../../src/TSlibs/tslib -mode:=user  $OPTIONS
#  DO xm =g ../../../../src/TSlibs/tslib -mode:=work -placement:=release $OPTIONS
  cd ../../../..
}


xm_through_xm() {
  rm enduser/tmp/*
  DO xm =p src/m/xm -mode:=enduser -ts_compatible:+ $OPTIONS -xdsdir=enduser/xds -env_target=linux -usestaticclibs:+
#  DO xm =p src/m/xm -mode:=work -placement:=release $OPTIONS -xdsdir=enduser/xds -env_target=linux
}


h2d(){
  rm enduser/tmp/*
  DO enduser/xds/bin/xm =p src/h2d/h2d -mode:=enduser $OPTIONS -usestaticclibs:+
}

posix_C(){
  rm enduser/tmp/*
  DO xm =p src/POSIX/posix.prj -sym_place:=$SYM_PLACE_C -decor=rt
  cp enduser/tmp/*.sym enduser/xds/sym/C
}

linux_C(){
  rm enduser/tmp/*
  DO xm =p src/Linux/linux.prj -sym_place:=$SYM_PLACE_C $OPTIONS
  cp enduser/tmp/*.sym enduser/xds/sym/C
}


x11_C(){
  rm -f enduser/tmp/*
  DO  xm =p src/X11/X11.prj -sym_place:=$SYM_PLACE_C -decor=rt
  DO  xm =p src/X11/Xm.prj -sym_place:=$SYM_PLACE_C -decor=rt
  DO  xm =p src/X11/XmP.prj -sym_place:=$SYM_PLACE_C -decor=rt

  cp enduser/tmp/*.sym enduser/xds/sym/C
}

arch(){
  VERSION_CONTAINER=src/u/xm.ob2
  VERSION=`grep "vers =" $VERSION_CONTAINER | sed "s/[^0123456789]//g"`
  DISTRIBUTION_NAME=`echo xds-c-$VERSION-$TYPE-linux | awk '{ print tolower($1);}'`
  cd enduser
  DO "tar -hcf - --exclude xds/licenses/xds.txt xds | gzip >../$DISTRIBUTION_NAME.tgz"
  cd ..
} 


clean() {
  rm -f enduser/tmp/*
  rm -f enduser/xds/bin/h2d 
  rm -f enduser/xds/bin/xm enduser/xds/lib/C/* enduser/xds/sym/C/*
  rm -f enduser/xds/C/C/* enduser/xds/include/C/*

#  rm -f enduser
}

build_C() {
  init
  libxds_C
  libmkf_C
  libts_C  
  libtsmkf_C
  xm_through_xm
  posix_C
#  linux_C
  x11_C
}

all() {
  build_C
  h2d
#  arch
}

half_C() {
  init
  libxds_C
  libmkf_C
  libts_C  
  libtsmkf_C
  xm_through_xm
#  arch
}

trial() {
  OPTIONS="-decor=rt -woff+ -trialversion:+ -gentrialsym:+ -name_separator:=__"
  TYPE=trial
}

OPTIONS="-decor=rt -woff+"
TYPE=enduser
SYM_PLACE_C=enduser/tmp:enduser/xds/sym/C:enduser/sym
TMP_PLACE_C=enduser/tmp
LIB_PLACE_C=enduser/xds/lib/C



for par in $*; do
  $par
done


echo  
echo ------
echo  DONE
echo ------
echo
