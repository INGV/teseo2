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

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "teseo_env.h"
#include "teseo_session.h"
#include "gtkaddons.h"

//static char SESSION_PATH[FILENAMELEN];
struct Session this_session;

extern   GtkWidget *preferencesdlg;
extern   GtkWidget *sessiondlg;

char init_session(){
    return align_session( sessiondlg, &this_session);
}


char align_session(GtkWidget * sessiondlg, struct Session * s){
    char ret=0;
    /**/
    //this_session.recordinfo
    //this_session.imageinfo
    //this_session.tracesinfo
    //this_session.preferences
    //this_session.notes

    return ret;
}

char load_session_widget(const char * filename, GtkWidget * sessiondlg){
    char ret=0;
    init_store_widget("teseo");
    ret = iface_load_rc( filename, sessiondlg );
    if (ret==0) g_message("booooh");
    return ret;
}

char save_session_widget(const char * filename, GtkWidget * sessiondlg){
    char ret=0;
    init_store_widget("teseo");
    ret = iface_save_rc( filename, sessiondlg );
    return ret;
}

char save_session(char * filename, struct Session * s){
    char ret=0;
    return ret;
}

char load_session(char * filename){
    char ret=0;
    char session_filename[FILENAMELEN]="aaa";
    char session_dlg_filename[FILENAMELEN]="bbb";
    char preferences_dlg_filename[FILENAMELEN]="ccc";
    char notes_filename[FILENAMELEN]="";
    FILE * f=NULL;
    gchar line[1024];
    char var[80]="";
    char content[FILENAMELEN]="";

    //g_message("Hello");

    if ( filexist(filename) ){
      f = fopen(filename,"r");
      if ( f != NULL ){

	 /*TODO use fgets!!*/
	 while(fgets (line, 1024,  f)){
         //g_message("Scanning line %s", line);
	   if ( strstr(line, "#") == NULL ) {
            sscanf(line,"%s = %s\n", var,content);
            //g_message("%s - %s", var,content);
	    if (strcmp("SessionFile",var) == 0)  {
	      strcpy(session_filename,content);
	      //g_message("found");
	    }
	    else{
	      //g_message("not found");
	    }
	    if ( strcmp("SessionDialogFile",var) == 0 )  strcpy(session_dlg_filename,content);
	    if ( strcmp("PreferencesDialogFile",var) == 0)  strcpy(preferences_dlg_filename,content);
	  }
	 }
	 g_message("%s %s %s",session_filename, session_dlg_filename, preferences_dlg_filename);
	 ret=load_session_widget(filename,sessiondlg);
      }
    }
    else {
      g_message("Wow");
    }

    return ret;
}


char new_session(char * filename, char * preferences_dlg_filename, char* notes){
    char ret=0;
    //char * dirname=NULL;
    char * session_filename=NULL;
    char * session_dlg_filename=NULL;
    ///char * preferences_dlg_filename=NULL;
    char * notes_filename=NULL;
    FILE * f=NULL;
    char order[] = "100";
    int  num_order=99;
    char external_preferences=FALSE;

    if (preferences_dlg_filename != NULL ){
      external_preferences=TRUE;
    }

    while (ret==0){
      num_order++;
      sprintf (order,"%d",num_order);

      session_filename         = create_name(filename,order,SESSION_EXT);
      session_dlg_filename     = create_name(filename,order,SES_DLG_EXT);
      if (external_preferences==FALSE)
          preferences_dlg_filename = create_name(filename,order,PREF_DLG_EXT);
      notes_filename           = create_name(filename,order,NOTES_EXT);

      if ( ! filexist(session_filename)){
        f = fopen(session_filename,"w");
        if ( (f != NULL) && (session_filename != NULL) && (session_dlg_filename != NULL) && ( preferences_dlg_filename != NULL) ) {
          ret=1;
          fprintf(f,"#Created by ...\n");
          fprintf(f,"SessionFile = %s\n",session_filename);
          fprintf(f,"SessionDialogFile = %s\n",session_dlg_filename);
          fprintf(f,"PreferencesDialogFile = %s\n",preferences_dlg_filename);
          fprintf(f,"NotesFile = %s\n",notes_filename);
          fclose(f);
        }
        else {
          g_message("Unable to open %s, session not created",session_filename);
        }


        /*Creating the dialogs real files*/
        if ( !iface_save_rc(session_dlg_filename,sessiondlg) )  g_error("Unable to save session dialog file") ;
	/*Only if it doesn't exist before (checked out before calling this function)*/
	if (external_preferences==FALSE) {
          if ( !iface_save_rc(preferences_dlg_filename, preferencesdlg) ) g_error("Unable to save preferences dialog file") ;
	}

	/*TODO notesfile*/
        if (notes!=NULL)strcpy(notes,notes_filename);

        if (session_filename) free(session_filename);
        if (session_dlg_filename) free(session_dlg_filename);
        if (preferences_dlg_filename && (external_preferences==FALSE)) free(preferences_dlg_filename);
        if (notes_filename) free(notes_filename);
      }
      else {
         g_warning("TODO: find another name, to not overwriting session");
      }
    }
    return ret;
}

/*
char new_session(char * filename){
    char ret=0;
    char * dirname=NULL;
    char * session_filename=NULL;
    char * session_dlg_filename=NULL;
    char * preferences_dlg_filename=NULL;
    char * notes_filename=NULL;
    FILE * f=NULL;
    char order[] = "100";
    int  num_order=99;

    while (ret==0){
      num_order++;
      sprintf (order,"%d",num_order);

      session_filename         = create_name(filename,order,SESSION_EXT);
      session_dlg_filename     = create_name(filename,order,SES_DLG_EXT);
      preferences_dlg_filename = create_name(filename,order,PREF_DLG_EXT);
      notes_filename           = create_name(filename,order,NOTES_EXT);

      if ( ! filexist(session_filename)){
        f = fopen(session_filename,"w");
        if ( (f != NULL) && (session_filename != NULL) && (session_dlg_filename != NULL) && ( preferences_dlg_filename != NULL) ) {
          ret=1;
          fprintf(f,"#Created by ...\n");
          fprintf(f,"SessionFile=%s\n",session_filename);
          fprintf(f,"SessionDialogFile=%s\n",session_dlg_filename);
          fprintf(f,"PreferencesDialogFile=%s\n",preferences_dlg_filename);
          fprintf(f,"NotesFile=%s\n",notes_filename);
          fclose(f);
        }
        else {
          g_message("Unable to open %s, session not created",session_filename);
        }


        //Creating the dialogs real files
        if ( !iface_save_rc(session_dlg_filename,sessiondlg) )  g_error("Unable to save session dialog file") ;
        if ( !iface_save_rc(preferences_dlg_filename, preferencesdlg) ) g_error("Unable to save preferences dialog file") ;
	///TODO notesfile


        if (session_filename) free(session_filename);
        if (session_dlg_filename) free(session_dlg_filename);
        if (preferences_dlg_filename) free(preferences_dlg_filename);
        if (notes_filename) free(notes_filename);
      }
      else {
         g_warning("TODO: find another name, to not overwriting session");
      }
    }
    return ret;
}*/


char* create_name(char * dirname, char* order, char* ext){
    char * name=NULL;
    char * name_noext=NULL;
    char * filename=NULL;
    char * p=NULL;
    char * token;
    const char delimiters[] = ".";
    char ptrptr[FILENAMELEN];

    filename = (char * ) malloc(sizeof(char)*FILENAMELEN);

    if ( filename != NULL) {

      p = get_environment_path( SESSIONPATH ); /* path to session files to be freed*/
      strcpy(filename,p);
      strcat(filename,"/");

      name = g_path_get_basename (dirname); /*basename without path, to be freed*/

      token = strpbrk(name, delimiters);


      //g_message("token %s", token);
      name_noext = g_strndup(name , strlen(name) - strlen(token) );

      //token = strtok_r(name, delimiters, &ptrptr );        /* token = filename without extension */
      //g_message("Name = %s",  token);
      strcat(filename,name_noext);
      strcat(filename,order);
      strcat(filename,ext);          /* adding extension */

      g_warning("Name = %s",  filename);
      if (name != NULL) g_free(name);
      if (name_noext != NULL) g_free(name_noext);
      if (p != NULL) free(p);
    }

    return filename;
}

char verify_session_name(char * filename, struct Session * s){
    char ret=0;
    return ret;
}

char save_preferences(char * filename, struct Session *s){
    char ret=0;
    return ret;
}

char load_preferences(char * filename, struct Session *s){
    char ret=0;
    return ret;
}

