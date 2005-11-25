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

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gtkaddons.h"
#include <glib.h>
#include <glib/gprintf.h>

#include <errno.h>

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

int is_marked_name(gchar * widget_name) {
	int ret=0;
	if ( strstr(widget_name,token) != NULL)
		ret=1;
	return ret;
}


int is_teseo_widget(GtkWidget * widget) {
	static const char token []="teseo" ;
	return is_token_widget(widget, token);
}

void iface_load_rc_recursive(gpointer data, gpointer user_data){
	guint n;
	GList *l;
	struct mydata *tmp=user_data;
	GtkTextBuffer* gtb;

	double s_value;
	//g_printf("looking for %s in container %s \n", (*tmp).w_name ,  gtk_widget_get_name(data) ) ;
	if ( strcmp( (*tmp).w_name,  gtk_widget_get_name(data)) ==0){
		//strcpy( user_data.w_content_to);
		if (GTK_IS_TOGGLE_BUTTON(data) ){
			// Contents of radiobuttons and togglebuttons
			if ( (GTK_IS_TOGGLE_BUTTON(data)) ) {
				//g_printf("found %s set to %s\n",(*tmp).w_name, (*tmp).w_content_to);
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
		if (GTK_IS_SPIN_BUTTON (data)  ){
			s_value=strtod ((*tmp).w_content_to,NULL);
			gtk_spin_button_set_value (GTK_SPIN_BUTTON(data), s_value);
		}
		if (GTK_IS_RANGE (data)  ){
			s_value=strtod ((*tmp).w_content_to,NULL);
			gtk_range_set_value (GTK_RANGE(data), s_value);
		}
                if (GTK_IS_TEXT_VIEW(data)){
			//g_printf("found %s  content buffer set to %s\n",(*tmp).w_name, (*tmp).w_content_to);
			gtb = gtk_text_view_get_buffer((GtkTextView *) data);
			gtk_text_buffer_set_text (gtb,  (*tmp).w_content_to, -1);
		}
	}
	else{
		if(GTK_IS_CONTAINER (data)) {
			l = gtk_container_get_children(data);
			n = g_list_length(l);
			if(l > 0) {
				g_list_foreach(l, iface_load_rc_recursive , user_data);
			}
		} else {
		}
	}
}

char iface_load_rc(const char * file_rc,  GtkWidget * parent_widget ){
	/*
	read the file, skip lines with # or ##, look for marked widgets,
	if found in parent_widget set the widget value.
	*/
	FILE *f=NULL;
	char ret=1;

	gchar * subline = NULL;
	char widget_name[80];
	char widget_content[80];
	char widget_type[80];
	char text_content[80*25*4]="";

	gchar line[1024];
	size_t lenght = 0;

	guint n;
	GList *l=NULL;

	struct mydata tmp;
	//debug g_printf("iface_load_rc::loading file %s\n", file_rc);
	f = fopen(file_rc, "rt");
	if(f) {
		while(fgets (line, 1024,  f)){
		// char * fgets(char * restrict str, int size, FILE * restrict stream);
			// if  contains	#	skip line
			if ( strstr(line, "GtkTextView") == NULL  && strstr(line, "#") == NULL  && strstr(line, "GtkWindow") ==NULL ) {
				sscanf(line,"%s %s %s\n", widget_type, widget_name,widget_content);
				subline = line + 2 +strlen(widget_type)+strlen(widget_name);
				lenght=strlen(subline);
				subline[lenght-1]='\0';
				//g_printf("%s %s %s\n", widget_type, widget_name,subline);
				if(GTK_IS_CONTAINER (parent_widget) ){
     				// fprintf(f, " - type = %s",  g_type_name(gtk_container_child_type(data)));
   					l = gtk_container_get_children(GTK_CONTAINER (parent_widget));
					n = g_list_length(l);
				}
				tmp.w_name=widget_name;
				//tmp.w_content_to=widget_content;
				tmp.w_content_to=subline;
				g_list_foreach( l, iface_load_rc_recursive , &tmp);
			}
			/*special case more lines for the widget content*/
			if ( strstr(line, "GtkTextView") != NULL) {
				sscanf(line,"\n%s %s\n", widget_type, widget_name);

				/*take all the text up to next TEXTEND*/
				while(fgets (line, 1024,  f)){
					if ( strstr(line, "TEXTEND") == NULL ) {
					strcat(text_content,line);
					}
					else{
					break;
					}
				}
				//g_printf("*** %s %s \nTEXT %s\n***\n",widget_type, widget_name, text_content);
				tmp.w_name=widget_name;
				//tmp.w_content_to=widget_content;
				tmp.w_content_to=text_content;
				g_list_foreach( l, iface_load_rc_recursive , &tmp);
			}
		}
		fclose(f);
	}
	 else {
	 	g_printf("\niface_load_rc::%s: %s\n", file_rc,strerror(errno));
		ret=0;
	}
	return ret;
}


char iface_load_list( struct param_struct * params,  GtkWidget * parent_widget ){
	/*
	look for widgets in params,
	if found in parent_widget set the widget value.
	*/

	char ret=1;

	char widget_name[P_NAME_LENGTH];
	char widget_content[P_VALUE_LENGTH];


	guint n;
	gulong i=0;
	GList *l=NULL;

	struct mydata tmp;

	//g_printf("iface_load_list:: started\n ");
	for (i=0;i<(*params).current;i++){

			strcpy(widget_name, (*params).name[i]);
			strcpy(widget_content,(*params).value[i]);
			//g_printf("%s %s\n", widget_name,widget_content);
			if(GTK_IS_CONTAINER (parent_widget) ){
				l = gtk_container_get_children(GTK_CONTAINER (parent_widget));
				n = g_list_length(l);
			}
			tmp.w_name=widget_name;
			tmp.w_content_to=widget_content;
			//g_printf("%s %s \n", tmp.w_name, tmp.w_content_to);
			g_list_foreach( l, iface_load_rc_recursive , &tmp);

	}
	return ret;
}



char iface_save_rc(const char * file_rc,  GtkWidget * parent_widget) {
	FILE *f=NULL;
	char ret=1;
	//debug g_printf("iface_save_rc::saving file %s\n", file_rc);
	f = fopen(file_rc, "wt");
	if(f) {
		fprintf(f, "# File created by iface_save_rc()\n");
		fprintf(f, "# $Id: gtkaddons.c,v 1.15 2005-11-25 11:29:48 ilpint Exp $\n");
		fprintf(f, "#\n");
		fprintf(f, "%s %s %s\n", GTK_OBJECT_TYPE_NAME(parent_widget), gtk_widget_get_name(parent_widget), gtk_widget_get_name(parent_widget));
		iface_save_rc_recursive(parent_widget, f);
		fclose(f);
	} else {
	 	g_printf("\niface_save_rc::%s: %s\n", file_rc,strerror(errno));
		ret=0;
	}
	return ret;
}

void iface_save_rc_recursive(gpointer data, gpointer user_data) {
	//navigate the widget saving names and values of childs
	guint n;
	GList *l;
	gchar * edits;
	FILE *f = user_data;
	GtkTextBuffer* gtb;
	GtkTextIter start, stop;

	if ( !is_marked_widget (data) ){
			// The line is commented
			fprintf(f, "# ");
	}
    //don't save two time parent window
	if ( strcmp(GTK_OBJECT_TYPE_NAME(data),"GtkWindow")!=0 ) {
		// fprintf(f, "\nname = %s", gtk_widget_get_name(data));
		fprintf(f, "%s %s ", GTK_OBJECT_TYPE_NAME(data), gtk_widget_get_name(data));

		if (GTK_IS_TOGGLE_BUTTON(data) || GTK_IS_EDITABLE (data) ||GTK_IS_TEXT_VIEW(data) || GTK_IS_RANGE (data)){
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

			//Contents of ranges
			if ( GTK_IS_RANGE (data) ) {
				fprintf (f, "%2.2f\n",gtk_range_get_value ( GTK_RANGE(data)));
			}

			//contens of textview
			if (GTK_IS_TEXT_VIEW(data)){
			  gtb = gtk_text_view_get_buffer((GtkTextView *) data);
			  gtk_text_buffer_get_bounds (gtb,&start,&stop);
			  fprintf (f,"\n%s\nTEXTEND\n", gtk_text_buffer_get_text ( gtb, &start, &stop, FALSE));
			}
		} else {
			// Other widgets
			// fprintf(f, "\n%s", gtk_widget_get_name(data));
			fprintf(f, "##UNKNOWN  value##\n");
		}
	}
	if(GTK_IS_CONTAINER (data)) {
		// fprintf(f, " - type = %s",  g_type_name(gtk_container_child_type(data)));
		l = gtk_container_get_children(data);
		n = g_list_length(l);
		// fprintf(f, " - nchildren = %d\n", n);
		if(l > 0) {
			g_list_foreach(l, iface_save_rc_recursive , f);
		}
	} else {
		//fprintf(f, "\n");
	}
}



void iface_display_rc_recursive(gpointer data, gpointer user_data){
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
			g_list_foreach(l, iface_save_rc_recursive , (gpointer) tab);
		}
	} else {
		//g_printf("\n");
	}
}


void print_iface(gpointer data, gpointer user_data) {
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
			g_list_foreach(l, print_iface, (gpointer) tab);
		}
	} else {
		g_printf("\n");
	}
}

void iface_list_delete(struct param_struct ** params ){

	gulong i=0;
	if((*params)!=NULL){
		for(i=0;i<(**params).current;i++){
			g_free((**params).name[i]);
			g_free((**params).value[i]);
		}
		g_free(*params);
	}
	*params=NULL;
}

void iface_list_print(struct param_struct * params ){

	gulong i=0;
	if( params!=NULL ){
		for(i=0;i<(*params).current;i++){
			g_printf(" %s %s \n", (*params).name[i], (*params).value[i]);
		}
	}
	else {
		g_printf("Warning:: params is Null\n");
	}
}

void iface_list(GtkWidget * parent_widget , struct param_struct ** params ){

	struct param_struct * lp = (struct param_struct*) g_malloc(sizeof(struct param_struct));
	//g_printf("iface_list:: widget %s\n", gtk_widget_get_name(parent_widget));
        (*lp).current=0;
	iface_list_recursive( parent_widget, lp);
        *params=lp;
}

/*!
iface_list_recursive browses recursively in a container and save in param_name the teseo widget name and in param_value its value
	it is called by iface_list
	\param data current widget
	\param user_data file descript pointer. File must be open.
*/


//void iface_list_recursive( GtkWidget * data, struct param_struct *ps){
void iface_list_recursive( gpointer data, gpointer ps){
	//navigate the widget saving names and values of childs
	guint n;
	GList *l;
	gchar * edits;
//	FILE *f = user_data;
	GtkTextBuffer* gtb;
	GtkTextIter start, stop;
	//g_printf("iface_list_recursive:: widget %s\n", gtk_widget_get_name(data));
	struct param_struct *lps = (struct param_struct *) ps;

	if ( is_marked_widget (data) ){
		//don't save two time parent window
		if ( strcmp(GTK_OBJECT_TYPE_NAME(data),"GtkWindow")!=0 ) {
			//debug g_printf("%s\n",gtk_widget_get_name(data));
			(*lps).name[(*lps).current]  = (gchar *) g_malloc(sizeof(gchar)*(1+P_NAME_LENGTH));
			(*lps).value[(*lps).current] = (gchar *) g_malloc(sizeof(gchar)*(1+P_VALUE_LENGTH));
			//ATTENTION
			//sprintf((*lps).name[(*lps).current], "%s$$%s", GTK_OBJECT_TYPE_NAME(data), gtk_widget_get_name(data));
			sprintf((*lps).name[(*lps).current], "%s", gtk_widget_get_name(data));
			if (GTK_IS_TOGGLE_BUTTON(data) || GTK_IS_EDITABLE (data) ||GTK_IS_TEXT_VIEW(data) || GTK_IS_RANGE (data)){
				// Contents of radiobuttons and togglebuttons
				if ( GTK_IS_TOGGLE_BUTTON(data) ) {
					if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data)) ) {
						sprintf ((*lps).value[(*lps).current], "%s", "TRUE");
					} else {
						sprintf ((*lps).value[(*lps).current], "%s", "FALSE");
					}
				}

				// Contents of entry and spinbutton
				if ( GTK_IS_EDITABLE (data) ) {
					edits=gtk_editable_get_chars ( (GtkEditable *) data, 0, -1);
					g_snprintf ((*lps).value[(*lps).current], P_VALUE_LENGTH,"%s", edits);
					g_free(edits);
				}

				//Contents of ranges
				if ( GTK_IS_RANGE (data) ) {
					g_snprintf ((*lps).value[(*lps).current],  P_VALUE_LENGTH, "%2.2f",gtk_range_get_value ( GTK_RANGE(data)));
				}

				//contens of textview
				if (GTK_IS_TEXT_VIEW(data)){
					gtb = gtk_text_view_get_buffer((GtkTextView *) data);
					gtk_text_buffer_get_bounds (gtb,&start,&stop);
					//fprintf (f,"\n%s\nTEXTEND\n", gtk_text_buffer_get_text ( gtb, &start, &stop, FALSE));
					g_snprintf ((*lps).value[(*lps).current], P_VALUE_LENGTH,"%s", gtk_text_buffer_get_text ( gtb, &start, &stop, FALSE));
				}
			} else {
				// Other widgets
				// fprintf(f, "\n%s", gtk_widget_get_name(data));
				//fprintf(f, "##Need catch widget value##\n");
				g_snprintf ((*lps).value[(*lps).current], P_VALUE_LENGTH,"%s", "##UNKNOWN  value##");
			}
			//g_printf("%s  %s the %u\n", (*lps).name[(*lps).current], (*lps).value[(*lps).current],(unsigned int)(*lps).current );
			(*lps).current=(*lps).current+1;
		}
	}

	if(GTK_IS_CONTAINER (data)) {
		// fprintf(f, " - type = %s",  g_type_name(gtk_container_child_type(data)));
		l = gtk_container_get_children(data);
		n = g_list_length(l);
		// fprintf(f, " - nchildren = %d\n", n);
		if(l > 0) {
			//g_printf("%s the %d\n",gtk_widget_get_name(data),(unsigned int)(*lps).current );
			g_list_foreach(l, iface_list_recursive , lps);
		}
	} else {
		//fprintf(f, "\n");
	}


}


