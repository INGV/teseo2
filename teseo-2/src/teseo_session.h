/* Teseo-2 Plug-in
 * Copyright (C) 2005  Stefano Pintore, Matteo Quintiliani (the "Authors").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE Authors BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Authors of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Authors.
 */

/*

The vectorising session includes:

Event info:
	Date: Event Date
	Channel code: 2 char
	Component code
	Station code
	Station location: lat, long, quote

Image info:
	XCFfilename
	Resolution,....

Traces info:

	SVG filename
	Trace type: event, time-marker
	Sampling rate: sample/mm
	First sample Time

Vectorising Preferences: vectorizing settings

Notes: User notes

*/

#ifndef TESEOSESSION
#define TESEOSESSION 1

#include "teseo_io.h"

struct teseoDate{
	long year;
	int month;
	int day;
};

struct teseoTime{
	int hours;
	int minutes;
	int seconds;
};

struct Location{
	double lat;
	double lon;
	double quote;
};

struct EventInfo{
	struct teseoDate EventDate;
	char ChCode[2];
	char CompCod;
	char StaCode[5];
	struct Location StaLocation;
};

struct ImageInfo{
	char XCFfilename[FILENAMELEN];
	long Res;
};

enum TraceType{EVENT,TIME_MARKER};

struct TracesInfo{
	char SVGFilename[FILENAMELEN];
	char tracetype;
	long SamplingRate;
	struct teseoTime FirstSampleTime;
};


struct Preferences{
 char dummy;
};

struct Notes{
	char  text[255];
};

struct Session{
	struct EventInfo   eventinfo;
	struct ImageInfo   imageinfo;
	struct TracesInfo  tracesinfo;
	struct Preferences preferences;
	struct Notes       notes;
};

/*!
save_session return 1 if session saving on file filename succeed
	\param char * filename
	\param struct Session * s
*/
char save_session(char * filename, struct Session * s);

/*!
load_session return 1 if session loading from file filename succeed
	\param char * filename
	\param struct Session * s
*/
char load_session(char * filename, struct Session * s);

/*!
get_teseo_environment_path return a string containing the teseo environment path, the .gimp-majorversion.minorversion
*/
char * get_teseo_environment_path();

/*!
create_teseo_environment_path return 1 if teseo environment path creation succeed
	\param char * filename

*/
char create_teseo_environment_path(char * filename);

/*!
save_preferences return 1 if current preferences saving on file filename succeed
	\param char * filename

*/
char save_preferences(char * filename);

#endif
