#!/bin/sh

# Teseo-2 binary installation script for UNIX system (Only Linux tested)
# This distribution should contain:
#	- teseo-2 (binary plug-in)
#	- README
#	- NEWS
#	- AUTHORS
#	- COPYING

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

if  (hash $TOOL) ; then
  #echo "$TOOL found"
  GIMPDATADIR=$($TOOL --gimpdatadir)
  GIMPPLUGINDIR=$($TOOL --gimpplugindir)
else
 echo "Teseo-2 installation abort .."
 echo "Please install $TOOL"
 exit
fi

TESEOBINDESTINATION=$GIMPPLUGINDIR/plug-ins
TESEODATADESTINATION=$GIMPDATADIR/teseo-2

mkdir -p $TESEODATADESTINATION && exit

cp -fR $TESEOHELP $TESEODATADESTINATION/ && exit
cp -fR $TESEOPIXMAPS $TESEODATADESTINATION/ && exit
cp -fR $TESEOBIN $TESEOBINDESTINATION/ && exit
