#!/bin/sh

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

Action() {
    for i in ../../template/$1/*.tg
    do 
      ./test.sh $i $1
    done
}


clean_rep.sh
Iterate template.txt  Action
