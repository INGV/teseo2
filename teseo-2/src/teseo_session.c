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
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_env.h"
#include "teseo_session.h"
#include "teseocallbacks.h"
#include "gtkaddons.h"

//static char SESSION_PATH[FILENAMELEN];
struct Session this_session;

char current_session[FILENAMELEN];
char current_dlg_session[FILENAMELEN];
char current_main_window[FILENAMELEN];



char load_widget(const char * filename, GtkWidget * dlg){
	char ret=0;

	gchar * tep = NULL;
	gchar * complete_filename=NULL;

	//debug g_message("Loading file %s", filename);
	tep = teseo_get_environment_path(SESSIONPATH);
	//g_message( "Teseo session path %s", tep);

	complete_filename = (gchar * ) g_malloc(sizeof(char)*FILENAMELEN);

	if ( filename != NULL) {
		strcpy(complete_filename,tep);
		strcat(complete_filename,G_DIR_SEPARATOR_S);
		strcat(complete_filename,filename);
		//debug g_message("Loading file %s", complete_filename);
		ret = iface_load_rc( complete_filename, dlg );
	}

	g_free(tep);
	g_free(complete_filename);

	return ret;
}


char save_widget(const char * filename, GtkWidget * dlg){
	char ret=0;
	gchar * tep = NULL;
	gchar * complete_filename=NULL;

	tep = teseo_get_environment_path(SESSIONPATH);
	//debug g_message( "Teseo session path %s", tep);

	complete_filename = (gchar * ) g_malloc(sizeof(char)*FILENAMELEN);
	if ( filename != NULL) {
		strcpy(complete_filename,tep);
		strcat(complete_filename,G_DIR_SEPARATOR_S);
		strcat(complete_filename,filename);
		//debug g_message("Saving file %s", complete_filename);
		ret = iface_save_rc( complete_filename, dlg );
	}

	g_free(tep);
	g_free(complete_filename);

	return ret;
}

char save_session(char * filename){

	char ret=0;
	GimpParasite* first_parasite = NULL;
	ret=      save_widget(current_dlg_session, dlg_session);
	ret=ret * save_widget(current_main_window, win_teseo);

	char str[]="Parassita !";

	if(gimp_image_parasite_find(teseo_image,"Test")!=NULL){
		//remove old parasite
		gimp_image_parasite_detach  (teseo_image, "Test");
		g_message("Removed old parasite");
	}
	else {
		g_message("There wasn't parasite Test");
	}
	//create parasite
	first_parasite = gimp_parasite_new ("Test",   GIMP_PARASITE_PERSISTENT,  strlen(str)+1,  str);

	//attach parasite
	gimp_image_parasite_attach  (teseo_image,  first_parasite);
	gimp_parasite_free (first_parasite);

	return ret;
}

char load_session(char * filename){

	char ret =0;
	char retp=0;
	char rets=0;

	gchar session_filename[FILENAMELEN]="aaa";
	gchar dlg_session_filename[FILENAMELEN]="bbb";
	gchar main_window_filename[FILENAMELEN]="ccc";

	FILE * f=NULL;
	gchar line[1024];
	gchar * app;
	char var[80]="";
	gchar * base_content=NULL;

	GimpParasite* first_parasite = NULL;
	gint num_parasites;
	gchar **parasites=NULL;

	if (filename==NULL) {
		g_message("Attempting to load a null filename");
	}
	else {
	//debug g_message("Attempting to load %s session",filename);
	if ( teseo_filexist(filename) ){

	f = fopen(filename,"r");
	if ( f != NULL ){
		while(fgets (line, 1024,  f)){
			if ( strstr(line, "#") == NULL ) {
				sscanf(line,"%s ", var);
				if (strcmp("SessionFile",var) == 0)  {
					/*TODO consistency check*/
					//strcpy(session_filename,content);
					app = (line + strlen("SessionFile") + 3);
					g_strdelimit  (app, "\n",0x00);
					//base_content= g_path_get_basename(app);
					base_content= app;
					strcpy(session_filename,base_content);
					//debug g_message("SessionFile %s",session_filename);
					strcpy(current_session,session_filename);
					//if (base_content) g_free(base_content);
				}
				if ( strcmp("SessionDialogFile",var) == 0 ) {
					//strcpy(dlg_session_filename,content);
					app = (line + strlen("SessionDialogFile") + 3);
					g_strdelimit  (app, "\n",0x00);
					//base_content= g_path_get_basename(app);
					base_content= app;
					strcpy(dlg_session_filename,base_content);
					// debugg_message("Session dialog file %s",dlg_session_filename);
					rets = load_widget(dlg_session_filename,dlg_session);
					if (rets==0) g_message("Corrupted %s",dlg_session_filename);
					//if (base_content) g_free(base_content);
				}
				if ( strcmp("MainWindowFile",var) == 0) {
					//strcpy(main_window_filename,content);
					app = (line + strlen("MainWindowFile") + 3);
					g_strdelimit  (app, "\n",0x00);
					base_content= app;
					strcpy(main_window_filename,base_content);
					//debug g_message("MainWindow file %s",main_window_filename);
					retp = load_widget(main_window_filename,win_teseo);
					if (retp==0) g_message("Corrupted %s",main_window_filename);
				}
			}
		}//while
		ret=rets*retp;
	}
	}
	else {
		g_warning("load_session: File %s not found",filename);
	}
	//Registering current session
	if(ret==1){
		strcpy(current_session,session_filename);
		strcpy(current_dlg_session,dlg_session_filename);
		strcpy(current_main_window,main_window_filename);
		//debug g_message("Session %s %s %s", current_session,current_dlg_session,current_main_window);
	}

	if (gimp_image_parasite_list  (teseo_image, &num_parasites, &parasites)) {
		//g_message("loking for %s",*parasites);
		//first_parasite=gimp_image_parasite_find  (teseo_image, *parasites);
		if ((first_parasite=gimp_image_parasite_find  (teseo_image, "Test"))!=NULL);
			g_message("Found the parasite Test=%s", (gchar *) gimp_parasite_data(first_parasite) );
		}
		else
		{
			g_message("No parasites found");
		}
	}

	return ret;
}


char new_session(char * filename, char * main_window_filename){
	char ret=0;

	gchar * session_filename=NULL;
	gchar * dlg_session_filename=NULL;
	gchar * bsession_filename=NULL;
	gchar * bdlg_session_filename=NULL;
	gchar * bdlg_preferences_filename=NULL;

	FILE * f=NULL;
	gchar order[] = "100";
	int  num_order=99;
	char external_preferences=FALSE;

	if (main_window_filename != NULL ){
	external_preferences=TRUE;
	}

	while (ret==0){
		num_order++;
		g_sprintf (order,"%d",num_order);

		session_filename         = create_name(filename,order,SESSION_EXT);
		dlg_session_filename     = create_name(filename,order,SES_DLG_EXT);

		if (external_preferences==FALSE)
			main_window_filename = create_name(filename,order,MAIN_WIN_EXT);

		if ( ! teseo_filexist(session_filename)){
			f = fopen(session_filename,"wt");
			if ( (f != NULL) && (session_filename != NULL) && (dlg_session_filename != NULL) && ( main_window_filename != NULL) ) {
				ret=1;
				bsession_filename=g_path_get_basename(session_filename);
				bdlg_session_filename=g_path_get_basename(dlg_session_filename);
				bdlg_preferences_filename=g_path_get_basename(main_window_filename);
				fprintf(f,"#Created by ...\n");
				fprintf(f,"SessionFile = %s\n",bsession_filename);
				fprintf(f,"SessionDialogFile = %s\n",bdlg_session_filename);
				fprintf(f,"MainWindowFile = %s\n",bdlg_preferences_filename);
				fclose(f);
			}
			else {
				g_message("Unable to open %s, session not created",session_filename);
			}
			//Creating the dialogs real files
			if ( !iface_save_rc(dlg_session_filename,dlg_session) )  g_error("Unable to save session dialog file") ;
			//Only if it doesn't exist before (checked out before calling this function)
			if (external_preferences==FALSE) {
				if ( !iface_save_rc(main_window_filename, win_teseo) ) g_error("Unable to save MainWindow file") ;
			}
		}
		else {
				//g_warning("TODO: find another name, to not overwriting session");
		}
	}//while
	//Registering current session
	if(ret==1){
		strcpy(current_session,bsession_filename);
		strcpy(current_dlg_session,bdlg_session_filename);
		strcpy(current_main_window,bdlg_preferences_filename);
	}

	g_free(bsession_filename);
	g_free(bdlg_session_filename);
	g_free(bdlg_preferences_filename);


	if (session_filename) g_free(session_filename);
	if (dlg_session_filename) g_free(dlg_session_filename);
	if (main_window_filename && (external_preferences==FALSE)) g_free(main_window_filename);

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

	filename = (gchar * ) g_malloc(sizeof(char)*FILENAMELEN);
	if ( filename != NULL) {

		p = teseo_get_environment_path( SESSIONPATH ); // path to session files to be g_freed
		strcpy(filename,p);
		//debug g_message("Creating name %s %s %s/n %s/n %s", dirname, order, ext,p,filename);

		strcat(filename,G_DIR_SEPARATOR_S);
		name = g_path_get_basename (dirname); //basename without path, to be g_freed

		token = strpbrk(name, delimiters);
		name_noext = g_strndup(name , strlen(name) - strlen(token) );

		//token = strtok_r(name, delimiters, &ptrptr );        // token = filename without extension
		//debug g_message("Name %s Name_noext = %s" ,name, name_noext );
		strcat(filename,name_noext);
		strcat(filename,order);
		strcat(filename,ext);          // adding extension

		if (name != NULL) g_free(name);
		if (name_noext != NULL) g_free(name_noext);
		if (p != NULL) g_free(p);
	}

	return filename;
}

gchar test_session(char * filename){

	GDir  * dir = NULL;
	gchar * basename=NULL;
	gchar * basename_noext = NULL;
	gchar * myentry = NULL;
	gchar * token = NULL;
	gchar * p=NULL;

	const char delimiters[] = ".xcf";
	gchar ret=0;

	//g_printf("test_session::start filename %s\n",filename );
	basename = g_path_get_basename (filename); //basename without path, to be g_freed
	p = teseo_get_environment_path( SESSIONPATH ); // path to session files to be g_freed

	//extract basename without extension
	token = g_strrstr(basename, delimiters);
	basename_noext = g_strndup(basename , strlen(basename) - strlen(token) );
	g_free(basename);

	dir = g_dir_open ( p, 0, NULL);
	//g_printf("test_session:: basename noext=%s path=%s\n",basename_noext, p );

	if ( dir!=NULL ) {
		while ( (myentry = g_strdup(g_dir_read_name (dir)) )  != NULL ){ //don't free entry
			if(myentry!=NULL){
				//g_printf("test_session::myentry %s\n",myentry );
				//First looking for basename
				if(myentry!=NULL){
					if (g_strrstr(myentry,basename_noext)!=NULL)
					{
						if(myentry!=NULL){
							//then looking for extension
							if(g_strrstr(myentry,SESSION_EXT)!=NULL){
								g_free(myentry);
								ret=1;
								break;
							}
						}
					}
				}
			}

			g_free(myentry);
		}//while
		g_dir_close(dir);
	}

	g_free(basename_noext);
	g_free(p);
	//g_printf("test_session::stop exiting %d\n", ret);
	return ret;
}
