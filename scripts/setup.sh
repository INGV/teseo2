#!/bin/sh

# Teseo-2 binary installation script for UNIX system (Only Linux tested)
# This distribution should contain:
#	- setup.sh (shell script installation, this file)
#	- teseo-2 (binary plug-in)
#	- README
#	- INSTALL
#	- NEWS
#	- AUTHORS
#	- COPYING
#	- example.xcf
#	- pixmaps directory
#	- help directory

# Teseo-2 GIMP Plug-in
# Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of the Authors of the
# Software shall not be used in advertising or otherwise to promote the
# sale, use or other dealings in this Software without prior written
# authorization from the Authors.

TESEOBIN="teseo-2"
TESEOHELP="help"
TESEOPIXMAPS="pixmaps"

TOOL="gimptool-2.0"

SYNTAX="Syntax: $0 [uninstall]"

echo ""

# Check parameters
if [ ! -z $2 ]; then
	echo $SYNTAX
	echo ""
	exit
fi

if [ ! -z $1 ] && [ "$1" != "uninstall" ]; then
	echo $SYNTAX
	echo ""
	exit
fi


# Check gimptool-2.0 utility
if  (hash $TOOL) ; then
  #echo "$TOOL found"
  GIMPDATADIR=$($TOOL --gimpdatadir)
  GIMPPLUGINDIR=$($TOOL --gimpplugindir)
else
 echo "Teseo-2 installation abort .."
 echo "Please install $TOOL"
 exit
fi

# Directories variables
TESEOBINDESTINATION=$GIMPPLUGINDIR/plug-ins
TESEODATADESTINATION=$GIMPDATADIR/teseo-2

# Install or Uninstall plug-in and associated files
if [ -z $1 ]; then

	echo "Creating directory $TESEODATADESTINATION"
	mkdir -p $TESEODATADESTINATION

	echo "Copying files..."
	cp -fR $TESEOHELP $TESEODATADESTINATION/ 
	cp -fR $TESEOPIXMAPS $TESEODATADESTINATION/ 
	cp -fR $TESEOBIN $TESEOBINDESTINATION/

else

	echo "Removing files..."
	rm -fr $TESEODATADESTINATION
	rm -f $TESEOBINDESTINATION/$TESEOBIN

fi

echo ""
