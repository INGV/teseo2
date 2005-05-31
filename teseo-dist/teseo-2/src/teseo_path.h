#ifndef TESEO_PATH
#define TESEO_PATH 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "teseo_types.h"

//31/05 #include "teseo_bezier.h"

/*Utilità*/
void neuronSismos_ruotastrokes(glong num_strokes, gdouble* strokes, gdouble** pstrokes_ruotato, gdouble angolo);

//31/05 perchè usa una f in bezier
//void concatena_path_path_array(gint32 g_image, glong path_n, gdouble *path);

/*Funzioni di I/O*/
gdouble * path_array_to_strokes( gdouble * path_array, glong n_details, glong * n_strokes);
gdouble * open_path_to_strokes(gint32 g_image, glong* n_strokes,  char * nome_path);
gdouble * open_path_to_array(gint32 g_image, glong* n_strokes,  char * nome_path);

void strokes_to_open_path(gint32 g_image, glong num_strokes, gdouble *strokes, char * nome_path);

/*Funzioni di manipolazione*/
void unisci_path( gint32 g_image );
void allinea_path( gint32 g_image );
void muovi_tracciato(gint32 g_image,gint x, gint y , gdouble rotate);
void concatena_path_strokes(gint32 g_image, glong num_strokes, gdouble *strokes);

	
#endif
