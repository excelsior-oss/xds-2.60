#!/bin/sh

REP_DIR=reports/$2
NAME=`expr match "$1" '.*\/\(.*\)\.'`
if [[ "$PATH" != *\.:* ]]; then
  export PATH=.:$PATH
fi

CheckDir() {
    if [ "$1" != "" ]
    then
      if [ ! -d `dirname $1` ]
      then
        CheckDir `dirname $1`
      fi

      if [ ! -d $1 ]
      then
        mkdir $1
      fi
    fi
}

echo   --- PROCESSING TEMPLATE $NAME ---
clean.sh
CheckDir $REP_DIR
../ctr/ctr $1 $REP_DIR/$NAME.rep 1>$REP_DIR/$NAME.log 2>&1

nfiles=0
for i in mod ob2
do
    nfiles=`expr $nfiles + $(ls tmp/$i/*.$i 2>/dev/null 1| wc -l)`
done

if [ $nfiles != 0 ]; then 
  echo \*\*\* FAIL \*\*\* \($nfiles tests\)
  if [ -d $REP_DIR/$NAME ]; then
    rm -rf $REP_DIR/$NAME;
  fi
  mkdir $REP_DIR/$NAME

  for i in mod def ob2
  do
    cp tmp/$i/*.$i $REP_DIR/$NAME >/dev/null 2>&1
  done
  cp comp.log $REP_DIR/$NAME.complog

else
  echo --- OK ---
#  rm $REP_DIR/$NAME.log $REP_DIR/$NAME.complog 1>/dev/null 2>&1

fi
