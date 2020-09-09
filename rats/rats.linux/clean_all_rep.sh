#!/bin/sh

clean()
{
  pushd $1 >/dev/null
  ./clean_rep.sh
  popd >/dev/null
  echo $1 is cleaned >> test_all.log
  echo $1 is cleaned 
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

	  
rm test_all.log >/dev/null 2>&1
Iterate testing.txt clean

echo All rep-files are cleared >> test_all.log
		    
		    
