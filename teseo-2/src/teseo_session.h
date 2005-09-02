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

#ifndef __TESEO_SESSION_H__
#define __TESEO_SESSION_H__ 1

#include "teseo_io.h"
#include "main.h"

#define SESSION_EXT  ".tsf"
#define SES_DLG_EXT  ".tsd"
#define MAIN_WIN_EXT ".tmw"

//#define NOTES_EXT    ".txt"
struct TeseoDate{
	long year;
	int month;
	int day;
};

struct TeseoTime{
	int hours;
	int minutes;
	int seconds;
};

struct Location{
	double lat;
	double lon;
	double quote;
};

struct RecordInfo{
	struct TeseoDate EventDate;
//	struct teseoTime EventTime;
	struct Location StaLocation;
	char stacode[5];
	char chcode[2];
	char compcod;
};

struct ImageInfo{
	char XCFfilename[FILENAMELEN];
	long res;
};

enum TraceType{EVENT, EVENT_TM, TM };

struct TracesInfo{
	char SVGFilename[FILENAMELEN];
	char tracetype;
	long samplingrate;
	struct TeseoTime FirstSampleTime;
};


struct Preferences{
 char dummy;
};

struct Notes{
	char  text[80*25];
};

struct Session{
	struct RecordInfo  recordinfo;
	struct ImageInfo   imageinfo;
	struct TracesInfo  tracesinfo;
	struct Preferences preferences;
	struct Notes       notes;
};

extern char current_session[FILENAMELEN];


/*!
init_session return 1 if correctly initialise current session structure
*/
char init_session();

/*!
init_session return 1 if correctly initialise the session structure s with data from the interface widget
*/
char align_session(GtkWidget * sessiondlg, struct Session * s);

/*!
load_session_widget return 1 if correctly initialise the session widget
*/
char load_session_widget(const char * filename, GtkWidget * sessiondlg);

char save_session_widget(const char * filename, GtkWidget * sessiondlg);

/*!
create_session_name return a newly allocated string containing a new valid session name.
the name is s.imageinfo.XCFfilename-XXX.tsf the function create a filename not already present in dirname
	\param char * dirname
	\param struct Session * s
*/
char* create_name(char * dirname, char* order, char* ext);

/*!
verify_session_name return 1 if filename could be a valid session name for s, 0 otherwise.
Valid name is s.imageinfo.XCFfilename-XXX.tsf
	\param char * dirname
	\param struct Session * s
*/
char verify_session_name(char * filename, struct Session * s);

/*!
save_session return 1 if session saving on file filename succeed
	\param char * filename
	\param struct Session * s
*/
char new_session(char * filename, char * dlg_preferences_filename);

/*!
save_session return 1 if session saving on file filename succeed
	\param char * filename
*/
char save_session(char * filename);

/*!
load_session return 1 if session loading from file filename succeed
	\param char * filename
	\param struct Session * s
*/
char load_session(char * filename);

/*!
save_preferences return 1 if current preferences saving on file filename succeed
	\param char * filename
	\param struct Session * s
*/
char save_preferences(char * filename, struct Session *s);

/*!
save_preferences return 1 if current preferences loading from file filename succeed
	\param char * filename
	\param struct Session * s
*/
char load_preferences(char * filename, struct Session *s);

char load_widget(const char * filename, GtkWidget * dlg);


#endif
