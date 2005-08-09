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
#include "teseocallbacks.h"
#include "gtkaddons.h"

//static char SESSION_PATH[FILENAMELEN];
struct Session this_session;

char current_session[FILENAMELEN];
char current_dlg_session[FILENAMELEN];
char current_dlg_preferences[FILENAMELEN];



char init_session(){
    return align_session( dlg_session, &this_session);
}


char align_session(GtkWidget * dlg_session, struct Session * s){
    char ret=0;
    
    //this_session.recordinfo
    //this_session.imageinfo
    //this_session.tracesinfo
    //this_session.preferences
    //this_session.notes

    return ret;
}


char load_widget(const char * filename, GtkWidget * dlg){
    char ret=0;

    ret = iface_load_rc( filename, dlg );

    return ret;
}


char save_widget(const char * filename, GtkWidget * dlg){
    char ret=0;

    ret = iface_save_rc( filename, dlg );

    return ret;
}

char save_session(char * filename){
    char ret=0;

    ret=      save_widget(current_dlg_session, dlg_session);
    ret=ret * save_widget(current_dlg_preferences, dlg_preferences);

    return ret;
}

char load_session(char * filename){
    char ret =0;
    char retp=0;
    char rets=0;

    char session_filename[FILENAMELEN]="aaa";
    char dlg_session_filename[FILENAMELEN]="bbb";
    char dlg_preferences_filename[FILENAMELEN]="ccc";

    FILE * f=NULL;
    gchar line[1024];
    char var[80]="";
    char content[FILENAMELEN]="";

    if ( teseo_filexist(filename) ){

      f = fopen(filename,"r");
      if ( f != NULL ){

	 while(fgets (line, 1024,  f)){

	   if ( strstr(line, "#") == NULL ) {
            sscanf(line,"%s = %s\n", var,content);
	    if (strcmp("SessionFile",var) == 0)  {
	      /*TODO consistency check*/

	      strcpy(session_filename,content);
	    }
	    else{
	      //g_message("not found");
	    }
	    if ( strcmp("SessionDialogFile",var) == 0 )  strcpy(dlg_session_filename,content);
	    if ( strcmp("PreferencesDialogFile",var) == 0)  strcpy(dlg_preferences_filename,content);
	  }
	 }
	 rets = load_widget(dlg_session_filename,dlg_session);
	 if (rets==0) g_message("Corrupted %s",dlg_session_filename);
	 retp = load_widget(dlg_preferences_filename,dlg_preferences);
	 if (retp==0) g_message("Corrupted %s",dlg_preferences_filename);
	 ret=rets*retp;
      }
    }
    else {
      g_message("You should open an existing session or create new one.");
    }
    //Registering current session
    if(ret==1){
     strcpy(current_session,session_filename);
     strcpy(current_dlg_session,dlg_session_filename);
     strcpy(current_dlg_preferences,dlg_preferences_filename);
    }

    return ret;
}


char new_session(char * filename, char * dlg_preferences_filename){
    char ret=0;

    char * session_filename=NULL;
    char * dlg_session_filename=NULL;

    FILE * f=NULL;
    char order[] = "100";
    int  num_order=99;
    char external_preferences=FALSE;

    if (dlg_preferences_filename != NULL ){
      external_preferences=TRUE;
    }

    while (ret==0){
      num_order++;
      sprintf (order,"%d",num_order);

      session_filename         = create_name(filename,order,SESSION_EXT);
      dlg_session_filename     = create_name(filename,order,SES_DLG_EXT);

      if (external_preferences==FALSE)
          dlg_preferences_filename = create_name(filename,order,PREF_DLG_EXT);

      if ( ! teseo_filexist(session_filename)){
        f = fopen(session_filename,"w");
        if ( (f != NULL) && (session_filename != NULL) && (dlg_session_filename != NULL) && ( dlg_preferences_filename != NULL) ) {
          ret=1;
          fprintf(f,"#Created by ...\n");
          fprintf(f,"SessionFile = %s\n",session_filename);
          fprintf(f,"SessionDialogFile = %s\n",dlg_session_filename);
          fprintf(f,"PreferencesDialogFile = %s\n",dlg_preferences_filename);
          fclose(f);
        }
        else {
          g_message("Unable to open %s, session not created",session_filename);
        }

        //Creating the dialogs real files
        if ( !iface_save_rc(dlg_session_filename,dlg_session) )  g_error("Unable to save session dialog file") ;
	//Only if it doesn't exist before (checked out before calling this function)
	if (external_preferences==FALSE) {
          if ( !iface_save_rc(dlg_preferences_filename, dlg_preferences) ) g_error("Unable to save preferences dialog file") ;
	}

      }
      else {
         g_warning("TODO: find another name, to not overwriting session");
      }
    }

    //Registering current session
    if(ret==1){
     strcpy(current_session,session_filename);
     strcpy(current_dlg_session,dlg_session_filename);
     strcpy(current_dlg_preferences,dlg_preferences_filename);
    }

    if (session_filename) g_free(session_filename);
    if (dlg_session_filename) g_free(dlg_session_filename);
    if (dlg_preferences_filename && (external_preferences==FALSE)) g_free(dlg_preferences_filename);

    return ret;
}


char* create_name(char * dirname, char* order, char* ext){
    char * name=NULL;
    char * name_noext=NULL;
    char * filename=NULL;
    char * p=NULL;
    char * token;
    const char delimiters[] = ".";
    // char ptrptr[FILENAMELEN];

    filename = (char * ) g_malloc(sizeof(char)*FILENAMELEN);

    if ( filename != NULL) {

      p = teseo_get_environment_path( SESSIONPATH ); // path to session files to be g_freed
      strcpy(filename,p);
      strcat(filename,"/");

      name = g_path_get_basename (dirname); //basename without path, to be g_freed

      token = strpbrk(name, delimiters);


      //g_message("token %s", token);
      name_noext = g_strndup(name , strlen(name) - strlen(token) );

      //token = strtok_r(name, delimiters, &ptrptr );        // token = filename without extension
      //g_message("Name = %s",  token);
      strcat(filename,name_noext);
      strcat(filename,order);
      strcat(filename,ext);          // adding extension

      g_warning("Name = %s",  filename);
      if (name != NULL) g_free(name);
      if (name_noext != NULL) g_free(name_noext);
      if (p != NULL) g_free(p);
    }

    return filename;
}

char verify_session_name(char * filename, struct Session * s){
    char ret=1;
    return ret;
}

char save_preferences(char * filename, struct Session *s){
    char ret=1;
    return ret;
}

char load_preferences(char * filename, struct Session *s){
    char ret=1;
    return ret;
}

