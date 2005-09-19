#!/bin/sh

#
# This script creates a binary distribution and it is associated to setup.sh
#
# This script requires:
#	- $CVSWORKTESEO2 variable
#
# And it does:
#	- create a tar.gz file containing the current version of Teseo-2 binary
#	
# Examples:
#	teseo_create_dist_curr_version_bin.sh 2.0.6-beta-bin-linux /local/mysoft
#		- create a file 'teseo-2.0.6-beta-bin-linux.tar.gz in /local/mysoft'
#	

if [ -z $CVSWORKTESEO2 ]; then
   echo ""
   echo "You have to set teseo-2 path before run this script !!!"
   echo ""
   exit
fi

if [ -z $2 ]; then
    echo ""
    echo "Syntax: $(basename $0) <distribution version X.X.X> <DIRLOCALARCHIVE>"
    echo ""
    exit
fi

# Set variables from script arguments
DISTNAME=$1
DIRLOCALARCHIVE=$2

# Check if $DIRLOCALARCHIVE exist
if [ ! -d $DIRLOCALARCHIVE ]; then
    echo "$DIRLOCALARCHIVE does not exist !"
    exit
fi

# Following variables denpend on TESEO cvs repository
MODULENAME=teseo-dist
DISTDIR=$CVSWORKTESEO2/$MODULENAME
SUBDIRTESEO2=$DISTDIR/teseo-2
SUBDIRGTKADDONS=$DISTDIR/gtk-addons

# Following variables determine file name distribution and tag for CVS repository
SOFTNAME=teseo
TESEODISTNAME=${SOFTNAME}-${DISTNAME}


##################
### Start script
##################

echo ""

echo "cd $DIRLOCALARCHIVE"
cd $DIRLOCALARCHIVE

echo "mkdir $TESEODISTNAME"
mkdir $TESEODISTNAME

echo ""

rsync -av $CVSWORKTESEO2/scripts/setup.sh $TESEODISTNAME/
rsync -av $SUBDIRTESEO2/src/teseo-2 $TESEODISTNAME/
rsync -av $SUBDIRTESEO2/README $TESEODISTNAME/
rsync -av $SUBDIRTESEO2/INSTALL $TESEODISTNAME/
rsync -av $SUBDIRTESEO2/AUTHORS $TESEODISTNAME/
rsync -av $SUBDIRTESEO2/COPYING $TESEODISTNAME/
rsync -av --exclude "Makefile*" --exclude "CVS" $SUBDIRTESEO2/docs $TESEODISTNAME/
rsync -av --exclude "Makefile*" --exclude "CVS" $SUBDIRTESEO2/pixmaps $TESEODISTNAME/
rsync -av --exclude "Makefile*" --exclude "CVS" $SUBDIRTESEO2/help $TESEODISTNAME/


tar cvfz $TESEODISTNAME.tar.gz $TESEODISTNAME
md5sum $TESEODISTNAME.tar.gz > $TESEODISTNAME.tar.gz.md5

rm -fr $TESEODISTNAME

echo ""

