/* Teseo-2 Plug-in
 * Copyright (C) 2005-2019  Stefano Pintore, Matteo Quintiliani (the "Authors").
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
 
#ifndef VAR_PLUG_IN
#define VAR_PLUG_IN 1

#include <gtk/gtk.h>


#define BRUSH_DFLT 1
#define BRUSH_USR  2

//#define EXEC_AUTO 100
//#define EXEC_USR  200

#define INTER_SI  1000
#define INTER_NO  2000
#define INTER_POLI 3000
#define INTER_MQ 4000
#define INTER_ORDER 2
#define INTER_NUM_PUNTI 4
#define INTER_SVDFIT_PUNTI 2
#define MIN_PUNTI 2
#define PASSO_BEZIER_DFLT 1

#define AUTOSAVE_YES 1
#define AUTOSAVE_NO 0

#define MINUTO 60000
#define AUTOSAVE_DFLT 1*MINUTO

#define MOSTRARE 1
#define NON_MOSTRARE 2
#define STEP 50
#define PUNTI_IN_MENO 5
#define ENTRY_WIDTH 75
#define SCALE_WIDTH 100

#define EXEC_PASSO_PASSO false
#define NUM_PATH 10

#define CAMPIONAMENTO_PROG_SI 1
#define CAMPIONAMENTO_PROG_NO 0

#define SCALATURA_SI 1
#define SCALATURA_NO 0


#define SAMPLE_SI 1
#define SAMPLE_NO 0


#define num_reti 5
//#define NOME_FILE_IMPOSTAZIONI "/.Impostazioni_teseo"

enum TipoFIT { LFIT = 0, SVDFIT};

static const guchar CANARY = 123;

enum tipo_file { DXF, DXF_SAMPLE, BZR, TIMEMARKER, TRACCIA, SISMA, SAC, SAC_XY };

enum tipo_inseguimento { NEURALE, COLORE_1, COLORE_2, COLORE_3 };

typedef struct {
//  gint brush_type;       /* tipo di pennello */
//  gint exec_type;       /* tipo di esecuzione*/
  gint inter_exec;      /* interpoliamo ? */
  gint inter_type;     /* tipo di interpolazione*/
  gint inter_order;     /* tipo di interpolazione*/
  gint inter_num_punti;     /* numero di punti coinvolti nell'interpolazione*/
  gint inter_num_punti_sv;     /* numero di punti coinvolti nell'interpolazione SVDFIT*/
  gint passo_bezier;
  gint mostra_messaggi;
  gint numero_passi;    /* passi fatti dalla rete*/
  gint punti_in_meno;

  gint num_path;         // numero di path da unire
  //31/05bool esec_passo_passo; // indicativo dell' esecuzione passo passo
  gboolean esec_passo_passo; // indicativo dell' esecuzione passo passo

//  gdouble ax_init;     /* likelihood of randomization (as %age) */
//  gdouble niterazioni;  /* repeat count */
  gchar filepesi[200];

  gint sw_campionamento_progressivo;
  gint scalatura;
  gint sample_auto;

} NeuronSismosVals;

typedef struct {
  gchar Rete[500];
  gchar Help[500];
} DefaultDir;


struct Glob_Var {
  // input
  char dir_teseo_data[500];
  char dir_teseo_bin[500];

  char DirTracce[200];
  char DirDXF[200];
  char DirTimeMark[200];
  char DirBZR[200];
  char DirSisma[200];
  char DirSac[200];
  char NomeFileImmagine[200];
  int AltezzaRete;
  int LarghezzaRete;
  int NumeroCoordinate;
  int PassoAvanzamento;

  // gestione output
  //31/05 GtkList *ListaTracce;

  char NomeFileEsempio[200];
  char NomeFileDxf[200];
  char NomeFileTimeMark[200];
  char NomeFileBzr[200];
  char NomeFileTraccia[200];
  char NomeFileSisma[200];
  char NomeFileSac[200];
  char NomeFileBzrAutosave[200];
  char NomeFileImpostazioni[200];
  //gestione path
  gint spin_x;
  gint spin_y;
  gdouble spin_rotate;

  gint tag_save;
  gint auto_save;
  gint time_auto_save;

  //31/05bool inseguimento_neurale;
  gboolean inseguimento_neurale;
  //31/05tipo_inseguimento ins_tipo;
  int ins_tipo;
};

struct Ins_manual_var
{

 int righe;         // sono le y
 int colonne;       // sono le x

 int marg_inf;      //margine superiore
 int col_princ;     //colore principale
 int marg_sup;      //margine inferiore

 int distanza;
 int passo;
};

struct ins_colore_snap
{
	int abilita_neurale;   // abilita snap neurale o meno; 0 No, 1 Si
	int righe;					// altezza del rettangolo di visuale
	int colonne;				// larghezza del rettangolo di visuale
	int colore;					// 0 per inseguire traccia nera, 255 per inseguire traccia bianca; in pratica è un valore booleano
	int spessore;				// spessore utilizzato durante l'inseguimento neurale
	int passo;
};


struct Impostazioni
{
//31/05 bool vis_menubar1;
//31/05  bool vis_table14;
//31/05 bool vis_hbox3;
//31/05 bool vis_hbox2;
 gboolean vis_menubar1;
 gboolean vis_table14;
 gboolean vis_hbox3;
 gboolean vis_hbox2;
};


#endif
