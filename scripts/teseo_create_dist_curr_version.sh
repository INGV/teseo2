#!/bin/sh

#
# This script requires:
#	- cvs2cl.pl in $PATH
#	- $CVSWORKTESEO2 variable
#
# And it does:
#	- create a tar.gz file containing the current version of Teseo-2 source code
#	- tag repository with the same distirbution version
#	- modify ChangeLog files
#	
# Examples:
#	teseo_create_dist_curr_version.sh unstable_20050816x1300000 /local/mysoft
#		- create a file 'teseo-unstable_20050816x1300000.tar.gz in /local/mysoft'
#		- tag teseo-dist repository with 'teseo_teseo-unstable_20050816x1300000'
#	
#	teseo_create_dist_curr_version.sh 2.0.4 /local/mysoft
#		- create a file 'teseo-2.0.4.tar.gz in /local/mysoft'
#		- tag teseo-dist repository with 'teseo_2_0_4'
#
#	If you have been creating a new version of Teseo rember to increment the version in source code !!!"
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
SUBDIRNEWUOA=$DISTDIR/newuoa

# Following variables determine file name distribution and tag for CVS repository
SOFTNAME=teseo
TESEODISTNAME=${SOFTNAME}-${DISTNAME}
TESEODISTNAMETAG=$(echo $TESEODISTNAME | tr . _ | tr - _)

# Following variables are used for cvs2cl.pl
FILECHANGELOG=ChangeLog
CVS2CLOPT="-r -f $FILECHANGELOG"
UFILE=$CVSWORKTESEO2/users
if [ -f $UFILE ]; then
    CVS2CLOPT="$CVS2CLOPT -U $UFILE"
else
    echo "* * * Warning: $UFILE not found !"
fi


##################
### Start script
##################

echo ""

# ChangeLog update for teseo-2
echo "cd $SUBDIRTESEO2"
cd $SUBDIRTESEO2
cvs edit $FILECHANGELOG
echo "cvs2cl.pl $CVS2CLOPT src/*.[ch]"
cvs2cl.pl $CVS2CLOPT src/*.[ch]  &&  cvs ci -m "ChangeLog for $TESEODISTNAME distribution" $FILECHANGELOG

echo ""

# ChangeLog update for gtk-addons
echo "cd $SUBDIRGTKADDONS"
cd $SUBDIRGTKADDONS
cvs edit $FILECHANGELOG
echo "cvs2cl.pl $CVS2CLOPT src/*.[ch]"
cvs2cl.pl $CVS2CLOPT src/*.[ch]  &&  cvs ci -m "ChangeLog for $TESEODISTNAME distribution" $FILECHANGELOG

echo ""

# ChangeLog update for newuoa`
echo "cd $SUBDIRNEWUOA"
cd $SUBDIRNEWUOA
cvs edit $FILECHANGELOG
echo "cvs2cl.pl $CVS2CLOPT src/*.[ch]"
cvs2cl.pl $CVS2CLOPT src/*.[ch]  &&  cvs ci -m "ChangeLog for $TESEODISTNAME distribution" $FILECHANGELOG

echo ""

echo "cd $DISTDIR"
cd $DISTDIR
cvs tag $TESEODISTNAMETAG
cd $DIRLOCALARCHIVE
cvs export -r $TESEODISTNAMETAG $MODULENAME
mv $MODULENAME $TESEODISTNAME
tar cvfz $TESEODISTNAME.tar.gz $TESEODISTNAME
md5sum $TESEODISTNAME.tar.gz > $TESEODISTNAME.tar.gz.md5
rm -fr $TESEODISTNAME

echo ""
echo "If you have been creating a new version of Teseo rember to increment the version in source code !!!"
echo ""

