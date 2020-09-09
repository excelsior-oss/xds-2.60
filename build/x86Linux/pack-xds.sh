#!/bin/bash

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


arch(){
  VERSION_CONTAINER=src/u/xm.ob2
  VERSION=`grep "vers =" $VERSION_CONTAINER | sed "s/[^0123456789]//g"`
  DISTRIBUTION_NAME=`echo xds-$VERSION-linux | awk '{ print tolower($1);}'`
  pushd enduser
  DO "tar -hcf - --exclude xds/readme/bench.txt --exclude xds/licenses/xdsx86.txt --exclude xds/licenses/xdsc.txt xds | gzip >../$DISTRIBUTION_NAME.tgz"
  popd
} 


arch

echo  
echo ------
echo  DONE
echo ------
echo
