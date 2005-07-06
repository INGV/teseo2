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
 
/* CVS log
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/06/27 13:19:18  ilpint
 * Corretta la documentazione
 *
 * Revision 1.8  2005/06/24 12:36:38  ilpint
 * Inserita funzione di inizializzazione
 *
 * Revision 1.7  2005/06/22 12:32:04  mtheo
 * Added comments for Doxygen
 *
 * Revision 1.6  2005/06/22 11:46:13  mtheo
 * Keyword Log
 *
 *
*/

#ifndef _MGLADE_RC_H
#define _MGLADE_RC_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

/*!
struct mydata:w_name is the widget name, w_content_to is the line content to setup widget
*/
struct mydata{
		gchar * w_name;
		gchar * w_content_to;
} ;



/*!
init_store_widget initialize the library token string
	\param const char * mytoken 
*/
int init_store_widget(const char * mytoken);




/*!
is_token_widget return 1 if widget name contain token value, else it returns 0
	\param widget pointer widget
	\param token string to match
*/
int is_token_widget(GtkWidget * widget, const char * token);

/*!
is_teseo_widget return 1 if widget name contain 'teseo', else it returns 0
	\param widget pointer widget
*/
int is_teseo_widget(GtkWidget * widget);

/*!
mglade_save_rc write in a file the widget values of a container 
	it calls mglade_save_rc_recursive
	\param file_rc file name
	\param parent_widget pointer widget where starting
*/
void mglade_save_rc(const char * file_rc,  GtkWidget * parent_widget);

/*!
mglade_save_rc_recursive browses recursively in a container and save in a file  the teseo widget name and its value
	it is called by mglade_save_rc
	\param data current widget
	\param user_data file descript pointer. File must be open.
*/
void mglade_save_rc_recursive(gpointer data, gpointer user_data);

/*!
mglade_display_rc_recursive browses recursively in a container and print to stdout the teseo widget name and its value
	\param data current widget
	\param user_data string for tabulating output
*/
void mglade_display_rc_recursive(gpointer data, gpointer user_data);

/*!
fmglade() test function. Browse in a cointainer and display to stdout widget name and its value
	\param data current widget
	\param user_data string for tabulating output
*/
void fmglade(gpointer data, gpointer user_data) ;

/*!
mglade_load_rc read in a file the widget values of a container it calls mglade_load_rc_recursive
	\param file_rc resourcefilename  
	\param user_data parent widget
*/
void mglade_load_rc(const char * file_rc,  GtkWidget * parent_widget );

/*!
mglade_load_rc_recursive browses in a cointainer looking for a widget to setup 
load the widget value.
  \param data current widget
	\param user_data  mydata structure 
*/
void mglade_load_rc_recursive(gpointer data, gpointer user_data);

#endif /* _MGLADE_RC_H */
