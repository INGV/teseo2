/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* CVS Log
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/07/06 13:06:07  ilpint
 * Creata libreria gtk-addons per trattare le interfacce gtk.
 * Dovrebbe funzionare anche autogen.sh.
 *
 * Revision 1.10  2005/06/27 13:06:36  ilpint
 * Inserita funzione che cerca nella finestra i widget da modificare e li modifica opportunamente,
 * secondo i nomi e i valori presenti nel file di risorsa.
 *
 * Revision 1.8  2005/06/22 11:46:13  mtheo
 * Keyword Log
 *
 *
*/

#include <gtk/gtk.h>
#include <string.h>
#define _GNU_SOURCE
       #include <stdio.h>
       #include <stdlib.h>
#include "mglade_rc.h"
#include <glib.h>
#include <glib/gprintf.h>


static char token [80]="";

int init_store_widget(const char * mytoken){
    int ret=1;
	strcpy(token,mytoken); 
    return ret;	
}


int is_token_widget(GtkWidget * widget, const char * token) {
	int ret=0;
	gchar name[80]; //corto?

	g_sprintf(name, "%s",gtk_widget_get_name(widget));
	if ( strstr(name,token) != NULL)
		ret=1;
	return ret;
}

int is_marked_widget(GtkWidget * widget) {
	int ret=0;
	gchar name[80]; //corto?
	g_sprintf(name, "%s",gtk_widget_get_name(widget));
	if ( strstr(name,token) != NULL)
		ret=1;
	return ret;
}


int is_teseo_widget(GtkWidget * widget) {
	static const char token []="teseo" ;
	return is_token_widget(widget, token);
}


void mglade_load_rc_recursive(gpointer data, gpointer user_data){
	guint n;
	GList *l;
    struct mydata *tmp=user_data;
	//g_printf("Cerco %s nel container %s \n", (*tmp).w_name ,  gtk_widget_get_name(data) ) ;
    
	if ( strcmp( (*tmp).w_name,  gtk_widget_get_name(data)) ==0){
		
		//		strcpy( user_data.w_content_to);
		if (GTK_IS_TOGGLE_BUTTON(data) ){
			// Contents of radiobuttons and togglebuttons	  
			if ( (GTK_IS_TOGGLE_BUTTON(data)) ) {  
				g_printf("Trovato %s viene posto a %s\n",(*tmp).w_name, (*tmp).w_content_to);			
				if ( strcmp( (*tmp).w_content_to, "TRUE" ) ==0) { 
					if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data))==FALSE ) 
						gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data),TRUE);
				} else {
					if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data))==TRUE  )  
						gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data),FALSE);
				}
			}	
		}
		if (GTK_IS_ENTRY (data) ){
			gtk_entry_set_text (GTK_ENTRY (data) ,(*tmp).w_content_to);
			//gtk_editable_insert_text (GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position);
		}	
	}
	else{
		if(GTK_IS_CONTAINER (data)) {
			l = gtk_container_get_children(data);
			n = g_list_length(l);
			if(l > 0) {
				g_list_foreach(l, mglade_load_rc_recursive , user_data);
			}
		} else {
		}
	}
}

void mglade_load_rc(const char * file_rc,  GtkWidget * parent_widget ){
	/*
	read the file, skip lines with # or ##, look for marked widgets, 
	if found in parent_widget set the widget value. 
	*/
	FILE *f;
	
	gchar * subline = NULL;
	char widget_name[80];
	char widget_content[80];	
	char widget_type[80];
	
    gchar * line = NULL;
	gchar * subline2=NULL;
	size_t len = 0;
	size_t lenght = 0;
    
	ssize_t read;
	guint n;
	GList *l;
	
    struct mydata tmp; 
	f = fopen(file_rc, "r");
	if(f) {
		//getdelim (lineptr, n, '\n', f);
		//while((read = getline(&line, &len, f)) != -1){
		while((read = getdelim (&line, &len, '\n', f)) != -1){
			// if  contains	#	skip line
			if ( strstr(line, "#") == NULL  && strstr(line, "GtkWindow") ==NULL ) {
				sscanf(line,"%s %s %s\n", widget_type, widget_name,widget_content);	
				subline = line + 2 +strlen(widget_type)+strlen(widget_name);
				lenght=strlen(subline);
				subline[lenght-1]='\0';
				g_printf("%s %s %s\n", widget_type, widget_name,subline);	
				if(GTK_IS_CONTAINER (parent_widget) ){
     				// fprintf(f, " - type = %s",  g_type_name(gtk_container_child_type(data)));
   					l = gtk_container_get_children(GTK_CONTAINER (parent_widget));
					n = g_list_length(l);
				}
				tmp.w_name=widget_name;
				//tmp.w_content_to=widget_content;
				tmp.w_content_to=subline;
				g_list_foreach( l, mglade_load_rc_recursive , &tmp);
			}	
		}
		if (line!=NULL)  free(line);	
		fclose(f);
	}
	 else {
		g_printf("\nError opening file \"%s\".\n", file_rc);
	}	
}

void mglade_save_rc(const char * file_rc,  GtkWidget * parent_widget) {
	FILE *f;
	f = fopen(file_rc, "wt");
	if(f) {
		fprintf(f, "# File created by mglade_save_rc()\n");
		fprintf(f, "# $Id: mglade_rc.c,v 1.2 2005-08-21 11:39:18 mtheo Exp $\n");
		fprintf(f, "#\n");
		fprintf(f, "%s %s %s\n", GTK_OBJECT_TYPE_NAME(parent_widget), gtk_widget_get_name(parent_widget), gtk_widget_get_name(parent_widget));
		mglade_save_rc_recursive(parent_widget, f);
		fclose(f);
	} else {
		printf("\nError opening file \"%s\".\n", file_rc);
	}
}

void mglade_save_rc_recursive(gpointer data, gpointer user_data) {
	//navigate the widget saving names and values of childs
	guint n;
	GList *l;
	gchar * edits;
	FILE *f = user_data;
	
	if ( !is_marked_widget (data) ){
			// The line is commented
			fprintf(f, "# ");
    }
    //don't save two time parent window
	if ( strcmp(GTK_OBJECT_TYPE_NAME(data),"GtkWindow")!=0 ) {
		// fprintf(f, "\nname = %s", gtk_widget_get_name(data));
		fprintf(f, "%s %s ", GTK_OBJECT_TYPE_NAME(data), gtk_widget_get_name(data));
	
		if (GTK_IS_TOGGLE_BUTTON(data) || GTK_IS_EDITABLE (data) ){
			// Contents of radiobuttons and togglebuttons	  
			if ( GTK_IS_TOGGLE_BUTTON(data) ) {  
				if ( gtk_toggle_button_get_active(data) ) { 
					fprintf (f, "TRUE\n");
				} else {
					fprintf(f, "FALSE\n");
				}	
				// fprintf(f, " - gtk_objectname = %s", GTK_OBJECT_TYPE_NAME(data));
				// fprintf(f, " - g_objectname = %s", G_OBJECT_TYPE_NAME(data));
				// fprintf(f, " - g_objecttype = %d", G_OBJECT_TYPE(data));
			}
		   
			// Contents of entry and spinbutton
			if ( GTK_IS_EDITABLE (data) ) {  
			    edits=gtk_editable_get_chars ( (GtkEditable *) data, 0, -1);
				//edits=gtk_entry_get_text ( (GtkEntry *) data);
				fprintf (f, "%s\n",  edits);
				g_free(edits);
			}
		} else {
			// Other widgets
			// fprintf(f, "\n%s", gtk_widget_get_name(data));
			fprintf(f, "##Need catch widget value##\n");
		}
	}
	if(GTK_IS_CONTAINER (data)) {
		// fprintf(f, " - type = %s",  g_type_name(gtk_container_child_type(data)));
		l = gtk_container_get_children(data);
		n = g_list_length(l);
		// fprintf(f, " - nchildren = %d\n", n);
		if(l > 0) {
			g_list_foreach(l, mglade_save_rc_recursive , f);
		}
	} else {
		//fprintf(f, "\n");
	}
}



void mglade_display_rc_recursive(gpointer data, gpointer user_data){
	//navigate the widget saving names and values of childs
	guint n;
	GList *l;
	char tab[255];
	char tabchar[10] = "";
	//char tabchar[10] = "  |___"; 
	char indent_tab[255];
	int indent;
	gchar * edits;
	
	indent=strlen(user_data);
	strcpy(indent_tab,user_data);
	
	memset(indent_tab, ' ', indent);
	//g_sprintf(tab, "%s%s", (char *) user_data, tabchar);
	g_sprintf(tab, "%s%s", indent_tab, tabchar);
	
	if(  is_teseo_widget (data) ) {	
		//g_printf("\n%sname = %s", tab, gtk_widget_get_name(data));
		g_printf("%s%s::%s=", tab, GTK_OBJECT_TYPE_NAME(data), gtk_widget_get_name(data));
		
		if (GTK_IS_TOGGLE_BUTTON(data) || GTK_IS_EDITABLE (data)  ){
			//contents of radiobuttons and togglebuttons	  
			if ( GTK_IS_TOGGLE_BUTTON(data) ) {  
				if ( gtk_toggle_button_get_active(data) ) { 
					g_printf ("TRUE\n");
				} else {
					g_printf("FALSE\n");
				}	
				//g_printf(" - gtk_objectname = %s", GTK_OBJECT_TYPE_NAME(data));
				//g_printf(" - g_objectname = %s", G_OBJECT_TYPE_NAME(data));
				//g_printf(" - g_objecttype = %d", G_OBJECT_TYPE(data));
			}
		   
			//contents of entry and spinbutton
			if ( GTK_IS_EDITABLE (data) ) {  
				edits=gtk_editable_get_chars ( (GtkEditable *) data, 0, -1);
				g_printf ("%s\n",  edits);
				g_free(edits);
			}
		} else {
			//other widgets
			//g_printf("\n%s%s", tab, gtk_widget_get_name(data));
			g_printf("\n");
		}
	}
	if(GTK_IS_CONTAINER (data)) {
		// g_printf(" - type = %s",  g_type_name(gtk_container_child_type(data)));
		l = gtk_container_get_children(data);
		n = g_list_length(l);
		//g_printf(" - nchildren = %d\n", n);
		if(l > 0) {
			g_list_foreach(l, mglade_save_rc_recursive , (gpointer) tab);
		}
	} else {
		//g_printf("\n");
	}
}


void fmglade(gpointer data, gpointer user_data) {
	guint n;
	GList *l;
	gchar tab[255];
	char tabchar[10] = "  |___";
	char indent_tab[255];
	int indent;
	
	indent=strlen(user_data);
	strcpy(indent_tab,user_data);
	
	memset(indent_tab, ' ', indent);
	//g_sprintf(tab, "%s%s", (char *) user_data, tabchar);
	g_sprintf(tab, "%s%s", indent_tab, tabchar);
	
	g_printf("%sname = %s", tab, gtk_widget_get_name(data));
	g_printf(" - gtk_objectname = %s", GTK_OBJECT_TYPE_NAME(data));
	g_printf(" - g_objectname = %s", G_OBJECT_TYPE_NAME(data));
	g_printf(" - g_objecttype = %d", G_OBJECT_TYPE(data));
 
	if(GTK_IS_CONTAINER (data)) {
		// g_printf(" - type = %s",  g_type_name(gtk_container_child_type(data)));
		l = gtk_container_get_children(data);
		n = g_list_length(l);
		g_printf(" - nchildren = %d\n", n);
		if(l > 0) {
			g_list_foreach(l, fmglade, (gpointer) tab);
		}
	} else {
		g_printf("\n");
	}
}
