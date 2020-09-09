#!/bin/bash

# XDSDIR=

if [ -z "$XDSDIR" ]
then
  CURRENDIR="$(cd "$(dirname "$0")";pwd)"
  if [ -e "$CURRENDIR/bin/xds" ]
  then
    XDSHOME="$CURRENDIR/bin/xds"
  elif [ -e "$CURRENDIR/../../bin/xds" ]
  then
    XDSHOME="$CURRENDIR/../../bin/xds"
  else
    echo "Path to XDS develpment system is undefined"
    exit 1
  fi

  export XDSDIR=$XDSHOME
  export PATH=./:$XDSDIR/bin:$PATH
fi