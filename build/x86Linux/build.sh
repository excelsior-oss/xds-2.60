#!/bin/bash

report() {

    if test $1 -gt 0; then 
        echo $2 build failed; 
    else 
        echo $2 build ok; 
    fi

}

setAttrs() {
    find ./enduser/ -type f -not -iname '*\.*' -exec dos2unix '{}' \;
    
    find ./enduser/ -type f -iname '*\.*' -exec chmod 664 '{}' \;
    find ./enduser/ -type f -not -iname '*\.*' -exec chmod 775 '{}' \;
    find ./enduser/ -type d  -exec chmod 775 '{}' \;

    find src/ -type f -iname '*\.*' -exec chmod 664 '{}' \;
}


setAttrs

if [ ! -e "enduser\xds" ]
then
  ./get.sh
fi

./makexds-x86.sh clean all
XDS_X86_RESULT=$?
./makexds-c.sh clean all
XDS_C_RESULT=$?
./pack-xds.sh
PACK_XDS_RESULT=$?

echo 
echo Build results
echo
report $XDS_X86_RESULT xds-x86
report $XDS_C_RESULT xds-c
report $PACK_XDS_RESULT xds-setup-package
