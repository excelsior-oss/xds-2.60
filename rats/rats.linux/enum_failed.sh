#!/bin/sh
# Enumerate the failed tests

out_file=failed_tests_`date +%Y.%m.%d`

cd $1/reports

  if [ -f $out_file ]; then
    mv $out_file $out_file"_old.txt"
  fi

  out_file=$out_file".txt"

  for i in `ls`
  do
    if [ -d $i ]; then
      echo Test group: $i >> $out_file
      echo ======================== >> $out_file

      for j in `ls $i`
      do
        if [ -d $i/$j ]; then
          echo $i\\$j >> $out_file
          echo ------------------------  >> $out_file

          for k in `ls $i/$j`
          do
            echo "   $i\\$j\\$k" >> $out_file
          done
        fi
      done
    fi

  done

cd ../..

