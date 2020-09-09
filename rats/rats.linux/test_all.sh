#!/bin/sh
REP_COUNT_FILE=rep_count.txt
REP_COUNT=0

if [[ "$PATH" != *\.:* ]]; then
  export PATH=.:$PATH
fi

test()
{
  echo $1 is being tested >> test_all.log
  echo $1 is being tested 
  cd $1
  if [ -d reports ]; then
    mv reports reports_$REP_COUNT
  fi
  ./test_all.sh
  cd ..
  enum_failed.sh $1
}

Iterate() {
    for i in `cat $1`
    do
      if [ "$i" != "" ]
      then
        fchar=`echo $i | cut -c1`
        if [ "$fchar" != ";" ]
        then
          $2 $i
        fi
      fi
    done
}

CheckRepCount() {
    if [ ! -f $REP_COUNT_FILE ]; then
      echo 0 > $REP_COUNT_FILE
    fi
    REP_COUNT=`cat $REP_COUNT_FILE`
    REP_COUNT=`expr $REP_COUNT + 1`
    echo $REP_COUNT > $REP_COUNT_FILE

    if   [ $REP_COUNT -lt 10 ]; then  
      REP_COUNT=00$REP_COUNT
    elif [ $REP_COUNT -lt 100 ]; then  
      REP_COUNT=0$REP_COUNT
    fi
}



rm test_all.log >/dev/null 2>&1
CheckRepCount

case $1 in
  "xdsn")  test xdsn;;
  "xdsc")  test xdsc;;
  "tscpn") test tscpn;;
  "tscpc") test tscpc;;
  "")      Iterate testing.txt test;;
esac

echo Tests are passed >> test_all.log
