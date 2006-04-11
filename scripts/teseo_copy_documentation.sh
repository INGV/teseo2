#!/bin/sh
#############################################
# Author: Matteo Quintiliani
#############################################
#
# This script requires:
#	- $CVSWORKTESEO2 variable
#
# And it does:
#	- copy html documentation from doc/project/teseo2/teseo2_user_manual to teseo-dist/teseo-2/help/en
#	- convert png image in jpg format
#
#############################################

if [ -z $CVSWORKTESEO2 ]; then
   echo ""
   echo "You have to set teseo-2 path before run this script !!!"
   echo ""
   exit
fi


# Set variables from script arguments
SRCDOC=$CVSWORKTESEO2/doc/project/teseo2/teseo2_user_manual
DSTDOC=$CVSWORKTESEO2/teseo-dist/teseo-2/help/en

cd $SRCDOC
cp -f *.html $DSTDOC/
cp -f *.css $DSTDOC/
SEDSTRING=""
for f in *.png; do convert -quality 100 $f $DSTDOC/$(echo $f | sed -e 's/\.png/\.jpg/'); done
cd $DSTDOC
replace_str.sh . "\.png" "\.jpg"
rm -f $(find . -name "*.old")


##################
### Start script
##################

