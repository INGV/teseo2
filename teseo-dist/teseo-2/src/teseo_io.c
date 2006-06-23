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

#include <unistd.h>
// windows compile patch #include <sys/types.h>
// windows compile patch #include <sys/wait.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "teseo_io.h"
#include "teseo_path.h"
#include "teseo_env.h"
#include "teseo_gimp_extends.h"
#include <locale.h>



//31/05#include "neuronutils.h"
//TODO spostare o modificare teseo_save_sisma_cm teseo_save_dxf_cm teseo_save_marcatempi_cm

/*To use for external commands*/
int my_system(char * command, char * const argv[200], char * const env[]  ){

 int status = 0;
/* windows compile patch
 int pid;
 int ret;
 pid=fork();

 if (pid==0)
 {
   //g_message("%s --- %s %s %s %s %s",command,argv[0],argv[1],argv[2],argv[3],argv[4]);
   ret=execve (command, argv , env);
   //g_message(command);
 }
 else
 {
   wait(&status);
   if(WIFEXITED(status)){
    g_message( "%d points saved.", WEXITSTATUS(status));
   }

 }
 */
 return status;
}



gint teseo_filexist(const char * filename)
{
 gint ret=0;  
   if (g_file_test(filename,G_FILE_TEST_EXISTS)){
    ret=1;
   }
  return ret;
}


gboolean teseo_if_file_exists_overwrite_dialog(const gchar *filename) {
    gboolean flag_save_file = TRUE;
    if(teseo_filexist(filename)) {

        GtkWidget *dialog_save = gtk_message_dialog_new_with_markup (
                NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                "File <i>%s</i> exists.\n\nDo you want overwrite it?",
                filename);

        switch (gtk_dialog_run (GTK_DIALOG (dialog_save)))
        {
            case GTK_RESPONSE_YES:
                flag_save_file = TRUE;
                break;
            case GTK_RESPONSE_NO:
                flag_save_file = FALSE;
                break;
            default:
                break;
        }
        gtk_widget_hide (dialog_save);
        gtk_widget_destroy(dialog_save);

    }
    return flag_save_file;
}


gint teseo_test_dir( char *dirname)
{
  gint ret=0;
  if ( teseo_filexist( dirname )) {
   if (g_file_test(dirname,G_FILE_TEST_IS_DIR )){
    ret=1;
   }
  }
  return ret;
}


/*Put on the image g_image the bezier path loaded from NomeFileDxf*/
void teseo_import_dxf ( gint32 g_image, char * NomeFileDxf  ) {
    FILE *fp = NULL;
    gchar str[20];
    gchar n_punti_str[10];
    gint n_punti;
    gint n_punti_tot=0;

    gint counter=0;

    gint i=0, a=0;

    gdouble xres;
    gdouble yres;
    gdouble dpi;
    gint image_height;

    gdouble x,y;
    gdouble *v_punti = NULL;

    float my_x;
    float my_y;

    gdouble vertex[200];

    vertex[0]=0.0;   //for negative coordinate in DXF
    vertex[1]=0.0;

    gint test_vertex=0;
    char jpolyline[2];


    setlocale(LC_NUMERIC, "C");

    gimp_image_get_resolution (g_image, &xres, &yres);
    //g_message("Risoluzione x=%f y=%f", xres, yres);

    image_height = gimp_image_height(g_image);
    //g_message("altezza y=%d", image_height);

    dpi = yres;

    fp = fopen( NomeFileDxf,"rt" );
    if(!fp) {
	g_message("File \"%s\" not found.", NomeFileDxf);
    } else {
	while(!feof(fp)) {
	    fscanf(fp,"%s\n",str);
	    if(!strncmp(str,"AcDbPolyline",12) && fscanf(fp,"%s\n",jpolyline) && !strncmp(jpolyline,"90",2)) {
		fscanf(fp,"%s\n70\n",n_punti_str);
		n_punti_tot = atoi(n_punti_str);
	    } else if( !strncmp( str, "AcDb2dVertex", 12 )  ) {
		n_punti_tot++;
	    }
	}
	fclose(fp);
    }

    v_punti = (gdouble *) g_malloc( ( sizeof(gdouble) ) * 2 *( n_punti_tot ));

    fp = fopen( NomeFileDxf,"rt" );
    if(!fp) {
	g_message("File \"%s\" not found.", NomeFileDxf);
    } else {
	while(!feof(fp)) {
	    fscanf(fp,"%s\n",str);
	    if(( !strncmp(str,"AcDbPolyline",12)) && (fscanf(fp,"%s\n",jpolyline)) && (!strncmp(jpolyline,"90",2)) ) {
		a=1;

		fscanf(fp,"%s\n70\n",n_punti_str);
		n_punti = atoi(n_punti_str);

		fscanf(fp,"0\n");

		for(i=counter; i < n_punti ;i++, counter++ ) {
		    fscanf(fp,"10\n");
		    fscanf(fp,"%f\n", &my_x );

		    fscanf(fp,"20\n" );
		    fscanf(fp,"%f\n",&my_y );
		    //printf("X=%f Y=%f \n",my_x, my_y );

		    x=my_x;
		    y=my_y;

		    v_punti[counter*2] = ( x/2.54*dpi );
		    v_punti[(counter*2+1)] = ( image_height - ( y*dpi/2.54 + 1 ) );
		}
	    } else if( !strncmp( str, "AcDb2dVertex", 12 )  ) {
		a=1;
		test_vertex=1;
		{

		fscanf(fp,"10\n");
		fscanf(fp,"%f\n", &my_x );
		fscanf(fp,"20\n" );
		fscanf(fp,"%f\n",&my_y );

		// printf("X=%f Y=%f \n",my_x, my_y );
		x=my_x;
		y=my_y;

		v_punti[counter*2] = ( x/2.54*dpi );
		v_punti[counter*2+1] = ( image_height - ( y*dpi/2.54 + 1 ) );

		if(v_punti[counter*2]<0)
		v_punti[counter*2]=v_punti[(counter-1)*2];

		counter++;

		}
	    } //ELSE
	}//WHILE

	if(!a)
	    g_message("File \"%s\" corrupted.", NomeFileDxf);
    }

    teseo_strokes_to_open_path( g_image, (glong) n_punti_tot, v_punti, "DXF Path imported");

    if(v_punti)
	g_free(v_punti);

    gimp_displays_flush();

    fclose(fp);
}

/*31/05
void Carica_Traccia ( gint32 g_image, char * NomeFileTraccia  )
{
double * strokes=NULL;
unsigned long n_strokes;
neuron_punti * punti_traccia;

punti_traccia=new neuron_punti(NomeFileTraccia);
strokes = punti_traccia->strokes(&n_strokes);
strokes_to_open_path( g_image, n_strokes, strokes, NomeFileTraccia);

if (strokes !=NULL ) delete strokes;

//gimp_displays_flush();
}
*/

/*Put on the image g_image the bezier path loaded from NomeFileBzr*/
void teseo_import_bzr ( gint32 g_image, char * NomeFileBzr  )
{
//g_message("%s",NomeFileBzr);
FILE *fp = NULL;
glong num_points, closed, draw, state, tipo,X,Y;
gdouble *path = NULL;
gchar pathname [200] = "";
gchar * app;
gchar linename [200] = "";
int i=0;

if( (fp = fopen(NomeFileBzr, "rt" )) )
{

fgets(linename, 200, fp);
//Salto Name:
app=linename+6;
strcpy(pathname,app);

//Test to exclude path with same name

fscanf(fp,"#POINTS: %ld\n" , &num_points);
fscanf(fp,"CLOSED: %ld\n" , &closed);
fscanf(fp,"DRAW: %ld\n", &draw);
fscanf(fp,"STATE:%ld\n", &state);
//g_message("nome=%s num_points=%d closed=%d draw=%d state=%d", pathname, num_points, closed, draw, state);

if ( closed==0 && draw==0 && state==2 )
{
//g_message("Path corretto");
path = (gdouble *) g_malloc( num_points * 3 * sizeof(gdouble) );
if (path != NULL) {
for ( i=0; i<num_points; i++)    {
fscanf(fp,"TYPE: %ld X: %ld Y: %ld\n", &tipo, &X, &Y);
path[i*3] = ( double ) X;
path[i*3+1] = ( double ) Y;
path[i*3+2] = ( double ) tipo;
}
gimp_path_set_points(g_image, pathname, 1, num_points * 3, path);
}
else {
g_message("Not enough free memory for path.");
}
}
else {
g_message("Wrong path.");
}

if(path) {
g_free(path);
}

fclose(fp);
}
else
{
g_message("File \"%s\" not found.", NomeFileBzr);
}
}


/*Output functions*/

/*Write an open path in old gimp bezier format*/
gint teseo_open_path_to_save(gint32 g_image,  char * nome_path, char * filename){
glong i=0, num_righe, len;
gdouble * points_pairs=NULL;
FILE * fp=NULL;
gint path_closed, num_path_point_details;
char s_tmp_app[255];
char * canc=NULL;
strcpy(s_tmp_app,nome_path);

//get the path
teseo_gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
//only open path are good
if (!path_closed){
//+3 perché per il primo punto scrive sei details, per gli altri nove ( x, y, tipo )
num_righe=num_path_point_details/3;

//     sprintf(s_tmp_app, "%s/tmp/path_saved.bak", getenv_teseo(TESEO_DATA));
//     sprintf(s_tmp_app, "%s", nome_file);

// fp=fopen("/tmp/path_saved.bak","wt");

/*Elimino # dal nome*/

canc = strpbrk(s_tmp_app, "#\n"); //punta al cancelletto o al \n finale
if ( canc!=NULL ){
len =  strlen(s_tmp_app) - strlen (canc) ; //lunghezza della sottostringa fino al # o al \n
//g_message("len= %d",len);
s_tmp_app[len]='\0';
     	len =  strlen(s_tmp_app);
     	//g_message("len= %d",len);
     }

     fp=fopen(filename,"wt");
     if(fp){
//      fprintf(fp,"Name: %s\n", nome_path);
      fprintf(fp,"Name: %s\n", s_tmp_app);
      fprintf(fp,"#POINTS: %ld\nCLOSED: 0\nDRAW: 0\nSTATE:2\n",num_righe);
     	for (i=0; i<num_righe; i++)    {
         fprintf(fp,"TYPE: %ld X: %ld Y: %ld\n",(glong) floor(points_pairs[i*3+2]), (glong) floor(points_pairs[i*3]),(glong)   floor(points_pairs[i*3+1]));
     	}
      fclose(fp);
     }

  }
g_free(points_pairs);
return 1;
}


//TODO: unify next two functions

/* Save the current path on the image g_image in filename in dxf format
 * scalatura = 1 -> shift first point of the trace in 0,0.0,0
 * Remember to resample before call.
*/

void teseo_save_path_dxf(gint32 g_image, char* filename, gint scalatura){
 //recupero il path in uno strokes, poi salva in dxf
 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
 /*Se esiste una traccia esegue*/
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //get path name
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = teseo_open_path_to_strokes(g_image, &n_strokes, pathname);
   teseo_strokes_dxf(g_image,filename, strokes, n_strokes, 1, scalatura); //tracciato
 }
 if (strokes)
  g_free(strokes);
}

/* Save the current timemarker path on the image g_image in filename in dxf format
 * scalatura = 1 -> shift first point of the trace in 0,0.0,0 */
void	teseo_save_path_timemarker(gint32 g_image, char* filename, gint scalatura){
 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
 //if a path exist
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //get the name of the current path
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = teseo_open_path_to_strokes(g_image, &n_strokes, pathname);
   teseo_strokes_dxf(g_image, filename, strokes, n_strokes, 0, scalatura ); //0 to save timemarker without resampling
 }
 if (strokes)
  g_free(strokes);
}

/*Save the current path on the image g_image in filename in sisma format*/
void teseo_save_path_sisma(gint32 g_image, char* filename){

 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
  //if a path exist
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
    //get the name of the current path
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = teseo_open_path_to_strokes(g_image, &n_strokes, pathname);
   //31/05   teseo_strokes_sisma(g_image, filename, strokes, n_strokes);
   teseo_strokes_sisma(g_image, filename, strokes, n_strokes, 0);
 }
 if (strokes)
  g_free(strokes);
}

/* Put a strokes vector in a dxf file
 * tracciato = 0 -> timemarker
 * tracciato = 1 -> trace
 */

void teseo_strokes_dxf(gint32 g_image, const char * file_dxf, gdouble* strokes , glong num_stroke, gint tracciato, gint scalatura){

  FILE*  fstrokes;
  gdouble xres;
  gdouble yres;
  gint image_height;

  gimp_image_get_resolution (g_image, &xres, &yres);
  image_height = gimp_image_height(g_image);
  fstrokes = fopen(file_dxf,"wt");
  if(!fstrokes)
    g_message("File \"%s\" not found.",file_dxf);
  else {
    if (strokes!=NULL)
     {

      if ( tracciato ){
      	teseo_save_dxf_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
      }
      else
      { //timemarker
     	teseo_save_marcatempi_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
      }
     }
     else
      {
      	// g_message("OOPS : What is it?");
      }
     fclose(fstrokes);
  }
}


/*  Put a strokes vector in a sisma file
 * xy = 0 -> only x coordinates
 * xy = 1 -> x,y coordinates
*/

void teseo_strokes_sisma(gint32 g_image, const char * file_sisma, gdouble* strokes , glong num_stroke, gchar xy){
  FILE*  fstrokes;
  gdouble xres;
  gdouble yres;
  gint image_height;

  gimp_image_get_resolution (g_image, &xres,&yres);

  image_height = gimp_image_height(g_image);

  fstrokes = fopen(file_sisma,"wt");
  if(!fstrokes)
    g_message("File \"%s\" not found.",file_sisma);
  else {
    teseo_save_sisma_cm(fstrokes,num_stroke, strokes, xres, image_height, xy );
    fclose(fstrokes);
  }
}

/*
   // TODO teseo_save_path_traccia
void teseo_save_path_traccia(gint32 g_image, char* filename){
 gdouble * strokes;
 glong n_strokes;
 gint num_paths;
 gchar pathname [80];
 gchar file_traccia [256];
 char nome_immagine [160];

 neuron_punti * punti;

 gimp_path_list (g_image, &num_paths);
   if (num_paths>0){
    //prendo il nome del path corrente
    strcpy(pathname, gimp_path_get_current(g_image));
    //traduco il path in strokes
    strokes = teseo_open_path_to_strokes(g_image, & n_strokes,  pathname);
    //traduco lo strokes in oggetto punti
    punti = new	neuron_punti(strokes, n_strokes);

    strcpy(nome_immagine,gimp_image_get_filename(g_image));
    punti->writefiletraccia(filename, nome_immagine, "Creato con Neuron Teseo");
    if (punti)
     delete punti;
  }
}
*/

/* Save the current open path in sac format
 * Remember: to use on a resampled path
 */

void teseo_save_path_sac(gint32 g_image, char* filename, char *dir_teseo_bin, gchar xy ){

 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gchar strcmd [200];

 char *argv[6];
 char argv0[100];
 char argv1[100];
 char argv2[100];
 char argv3[100];
 char argv4[100];

 char *env[]={"SACAUX=/",NULL};  //need SACAUX but don't use it

 gchar filename_tmp [255];

//31/05  sprintf(filename_tmp, "%s/tmp/sisma.tmp", getenv_teseo(TESEO_DATA));
 //TODO definire l'ambiente o prendere getenv_teseo e neuron_parse da teseo1
 g_sprintf(filename_tmp, "%s/sisma.tmp", teseo_get_environment_path(TMPPATH) );
 gint num_paths=0;
 gdouble xres, yres, passo;

 gimp_image_get_resolution (g_image, &xres, &yres);
 passo=2.54/xres;

  gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //get current path name
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = teseo_open_path_to_strokes(g_image, &n_strokes, pathname);

   //temporary saving in a sisma file
   teseo_strokes_sisma(g_image, filename_tmp, strokes, n_strokes, xy);
   //convert sisma file in sac
   g_sprintf(strcmd, "%s/bin/sisma_to_sac.exe" , dir_teseo_bin );

   /*Prepara la lista di argomenti*/
	 argv[0]=argv0;
	 argv[1]=argv1;
	 argv[2]=argv2;
	 argv[3]=argv3;
	 argv[4]=argv4;
	 argv[5]=NULL;

   sprintf(argv[0], "%s",strcmd );

   if (xy){
     sprintf(argv[1], "-u");
   }
   else
   {
     sprintf(argv[1], "-e");
   }

   sprintf(argv[2], "%s", filename_tmp );
   sprintf(argv[3], "%s", filename);
   sprintf(argv[4], "%f", passo);

 	 /*call to driver sisma_to_sac*/
 	 my_system (strcmd,argv,env);

 }
 if (strokes)
  g_free(strokes);
}

void teseo_save_path_ascii(gint32 g_image, char* filename) {
    gint i;
    gdouble * points_pairs=NULL;
    gint path_closed, num_path_point_details;
    FILE *fascii=NULL;
    gdouble xres, yres, xfactor, yfactor;
    gint32 image_height = gimp_image_height(g_image);

    if(teseo_gimp_path_get_points (g_image, gimp_path_get_current(g_image), &path_closed, &num_path_point_details, &points_pairs)) {

        gimp_image_get_resolution (g_image, &xres, &yres);
        xfactor = 25.4 / xres;
        yfactor = 25.4 / yres;

        fascii=fopen(filename, "wt");
        if(fascii) {
            for(i=0; i<num_path_point_details; i+=3) {
                if(points_pairs[i+2]==1.0) {
                    g_fprintf(fascii, "%f %f\n", xfactor * points_pairs[i], yfactor * (image_height - points_pairs[i+1]));
                }
            }
            fclose(fascii);
        } else {
            g_message("teseo_save_path_ascii(): unable to write in \"%s\" in ASCII format!", filename);
        }
    }

    g_free(points_pairs);
}

void teseo_import_path_ascii( gint32 g_image, char * NomeFileAscii, gint tm ) {
	//g_printf("Importing %s\n",NomeFileAscii);
	FILE *fascii = NULL;
	glong DIM_VECT=10000;
	glong num_points=DIM_VECT;
	gfloat X,Y;
	gdouble *path = NULL;
	gdouble *appath = NULL;
	gdouble xres, yres, xfactor, yfactor;
	gint i=0,k;
	gint fscanf_ret=0;

	gint32 image_height = gimp_image_height(g_image);
        gimp_image_get_resolution (g_image, &xres, &yres);

	xfactor = 25.4 / xres;
        yfactor = 25.4 / yres;

	if( (fascii = fopen(NomeFileAscii, "rt" )) )
	{
		path = (gdouble *) g_malloc( num_points * sizeof(gdouble) );

		if (path != NULL) {
			while ( ((fscanf_ret=fscanf(fascii,"%f %f\n", &X, &Y)) !=  EOF) && (fscanf_ret!=0) )
			{
				for ( k=1; k<4; k++){
					if ( !(i==0 && k==1) ) {
						path[ i*9+ 3*k -6    ]   =   ( double )  X / xfactor;
						path[ i*9+ 3*k -6 +1 ]   = - (( double ) Y / yfactor) + image_height;
						path[ i*9+ 3*k -6 +2 ]   =   ( double ) ( k==2 ) ? 1 : 2;
						//g_printf("X=%f Y=%f\n",path[ i*9+ 3*k -6    ],path[ i*9+ 3*k -6 +1 ]);
					}
				}
				if( (i+2)*9-4 >= num_points-1){
					num_points+=DIM_VECT;
					appath=g_try_realloc(path, num_points * sizeof(gdouble) );
					if (appath!=NULL) {
						if(appath!=path){
							//g_printf("Realloc %p %p\n", path,appath);
							path=appath;
							appath=NULL;
						}
						else {
						}
					}
					else{
						g_warning("teseo_import_path_ascii: Not enough memory");
					}
				}
				i++;
			}
			//g_printf("%d points read in vector of %d double\n",i,i*9-3);
			if (tm==0){
				gimp_path_set_points(g_image, "ASCII Path imported", 1, i*9-3 , path);
			}
			else{
				gimp_path_set_points(g_image, "Timemark path imported", 1, i*9-3 , path);
			}
		}
		else {
			g_warning("teseo_import_path_ascii: Not enough memory");
		}
		if(path) {
			g_free(path);
		}
		fclose(fascii);
	}
	else
	{
		g_message("File \"%s\" not found.", NomeFileAscii);
	}
}


/* Import a timemark file*/
void teseo_import_timemark ( gint32 g_image, char * NomeFileTimeMarker )
{

 FILE *fp = NULL;
 gdouble *strokes = NULL;
 gdouble xres,yres;
 gdouble dpi;
 gint image_height;
 int a=0,b=0,c=0;
 long counter=0;
 long counter_void;
 long counter0 = 32-1;
 double y_zero=0.0;
 double x1,y1,x2,y2;

 int i=0;

 if( (fp = fopen(NomeFileTimeMarker, "rt" )) )
 {
	/*Header*/
	fscanf(fp,"%d\nSECTION\n%d\nENTITIES\n%d\n", &a, &b, &c);

	if ( a==0 && b==2 && c==0 )
	{

	  while( fscanf(fp,"LINE\n5\n%ld\n6\nContinuous\n62\n3\n100\nAcDbLine\n10\n%lf\n20\n%lf\n30\n0.0\n11\n%lf\n21\n%lf\n31\n0.0\n0\n", &counter, &x1, &y1, &x2, &y2 ) == 5 ){
	  	if ( counter == 32 ) y_zero=y1;
	  }

  	fclose(fp);
  	fp = fopen(NomeFileTimeMarker, "rt");
	  counter = counter-counter0;

	  fscanf(fp,"%d\nSECTION\n%d\nENTITIES\n%d\n", &a, &b, &c);

   	strokes = (gdouble *) g_malloc( counter * 2 * sizeof(gdouble) );
   	if (strokes != NULL) {
   	  gimp_image_get_resolution (g_image, &xres,&yres);
   	  dpi=xres;
   	  image_height = gimp_image_height(g_image);

   	  for ( i=0; i<counter; i++)    {

           fscanf(fp,"LINE\n5\n%ld\n6\nContinuous\n62\n3\n100\nAcDbLine\n10\n%lf\n20\n%lf\n30\n0.0\n11\n%lf\n21\n%lf\n31\n0.0\n0\n", &counter_void, &x1, &y1, &x2, &y2 );

           strokes[i*2] = x1*dpi/2.54;
           strokes[i*2+1] = image_height - y1 * dpi / 2.54 - 1;
           //g_message("X=%lf Y=%lf", strokes[i*2], strokes[i*2+1] );
   	  }

       teseo_strokes_to_open_path( g_image, (glong) counter, strokes, "Imported Path");

  	  }
    else {
  	 	g_message("Not enough free memory for timemark path.");
    }
	}
 	else {
 		g_message("File corrupted.");
 	}

 	if(strokes) {
  	g_free(strokes);
  }

  fclose(fp);
 }
 else
 {
  g_message("File \"%s\" not found.", NomeFileTimeMarker);
 }
}



/*NEW entries*/

void teseo_save_dxf(FILE * fp, gint num_punti, gdouble *vet_punti){

int i=0;

fprintf(fp,
"0\n\
SECTION\n\
2\n\
ENTITIES\n\
0\n\
LWPOLYLINE\n\
5\n\
2B\n\
100\n\
AcDbEntity\n\
8\n\
1\n\
6\n\
CONTINUOUS\n\
62\n\
3\n\
100\n\
AcDbPolyline\n\
"
);//intestazione

fprintf(fp,"90\n%d\n70\n"
, num_punti);   //numero punti

fprintf(fp,"0\n");


for (i=0; i<num_punti; i++) {
fprintf(fp,"10\n%ld\n20\n%ld\n", (glong) (vet_punti[i*2]), (glong) (vet_punti[i*2+1]));
}

fprintf(
fp,"\
0\n\
ENDSEC\n\
0\n\
EOF\n"
);  // chiudo il file

}

//dpi risoluzione in DPI, height altezza immagine in pixel
/* initialized_old
void teseo_save_dxf_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start=1){
*/



/* Save a vector in dxf format, in centimeters.
 * dpi resolution, height the height of the image in pixel
 */

void teseo_save_dxf_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start){
double x_zero=0.0;
double y_zero=0.0;
//double x,y;

long i=0;

if( zero_start ) {
	x_zero=vet_punti[0];
	y_zero= height - vet_punti[1] - 1 ;
}
//Header
fprintf(fp,
"0\n\
SECTION\n\
2\n\
ENTITIES\n\
0\n\
LWPOLYLINE\n\
5\n\
2B\n\
100\n\
AcDbEntity\n\
8\n\
1\n\
6\n\
CONTINUOUS\n\
62\n\
3\n\
100\n\
AcDbPolyline\n\
"
);

fprintf(fp,"90\n%d\n70\n", num_punti);   //# points

fprintf(fp,"0\n");


for (i=0; i<num_punti; i++) {
fprintf(fp,"10\n%f\n20\n%f\n",  (vet_punti[i*2] - x_zero)*2.54/dpi,  (height - vet_punti[i*2+1]- 1 - y_zero)*2.54/dpi );
}

fprintf(
fp,"\
0\n\
ENDSEC\n\
0\n\
EOF\n"
);  //tail

}

/* initialized_old
void teseo_save_marcatempi_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start=1){
*/
void teseo_save_marcatempi_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start){

long counter = 32;
double h_seg=10.0;
double x_zero=0.0;
double y_zero=0.0;
double x,y;

long i=0;

if( zero_start ) {
	x_zero= vet_punti[0];
	y_zero= height - vet_punti[1] - 1 ;
}


//Header

fprintf(fp,
"0\n\
SECTION\n\
2\n\
ENTITIES\n\
0\n"
);

for (i=0; i<num_punti; i++) {
//line sections

x=(vet_punti[i*2]-x_zero) * 2.54 / dpi;
y=(height - vet_punti[i*2+1] - 1 - y_zero) * 2.54/dpi;

fprintf(fp,
"LINE\n\
5\n\
%ld\n\
6\n\
Continuous\n\
62\n\
3\n\
100\n\
AcDbLine\n\
10\n\
%f\n\
20\n\
%f\n\
30\n\
0.0\n\
11\n\
%f\n\
21\n\
%f\n\
31\n\
0.0\n\
0\n\
",
counter, x, y, x, y+h_seg);

counter++;

}

fprintf(fp,
"ENDSEC\n\
0\n\
EOF\n"
);  //Tail
}

//Salvataggio in formato sisma
//dpi risoluzione in DPI, height altezza immagine in pixel
/* initialized_old
void teseo_save_sisma_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar xy=0){
*/

/* Save the vector in sisma format, dpi resolution, height of the image in pixel, 
    xy to have x,y couples
*/

void teseo_save_sisma_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar xy){

gdouble media=0;
int i=0;

for (i=0; i<num_punti; i++) {
	media +=  ((height - vet_punti[i*2+1]-1)) ;
}
media/=num_punti;

for (i=0; i<num_punti; i++) {
	fprintf(fp,"%f\n", ( (height - vet_punti[i*2+1]-1 - media) * 2.54 / dpi) );
	if ( xy )  fprintf(fp,"%f\n", ( (vet_punti[i*2] - vet_punti[0] ) * 2.54 / dpi) )  ;
}

}


/*Salvataggio in formato sac*/

//extern void  wsac1 ( char * kname , float * teseo_strokes_sac , int * max , float * inizio , float * passo_delta , int * nerr , int *kname_num ) ;


//extern void wsac1( char * kname, float yarray[] , long int *nlen, float *beg,  float  *del, long int *nerr, long int kname_s);

/*
void wsac1(kname, yarray, nlen, beg, del, nerr, kname_s)
                        char     *kname;
                        float    yarray[];
                        long int *nlen;
                        float    *beg;
                        float    *del;
                        long int *nerr;
                        long int kname_s;
*/

//Salvataggio in formato sac
//dpi risoluzione in DPI, height altezza immagine in pixel
/*void teseo_save_sac_cm( char * file_sac, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height){
  long int max = 100 , j , nerr=0 ;
  float yfunc[ max ] , x , beg = 0. , del = 0.02 ;
  for ( j = 0 , x = beg ; j < max ; j++ , x += del )
        yfunc[ j ] = exp ( -x ) ;
  newhdr();
  wsac1 ( file_sac , yfunc , &max , &beg , &del , &nerr , strlen ( file_sac ) ) ;
  g_message("errore : %d",nerr);
}
*/

/*TODO test !!!*/
void teseo_save_sac_cm( char * file_sac, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height){

 float media=0;
 float * teseo_strokes_sac;
 float CANARY=-123.456789e6;
 long int lstrokes, lun_name;
 int i=0;

 g_message("num_punti %d, dpi %f, height %d ", num_punti, dpi, height);
 for (i=0;i<num_punti*2; i++ ){
  g_message("vet_punti[%d] : %f", i, vet_punti[i]);
 }

 lstrokes=num_punti;
 // TODO why is not g_free called for teseo_strokes_sac?
 teseo_strokes_sac = (float *) g_malloc(sizeof(float) * ( lstrokes + 1 ));
 if(!teseo_strokes_sac) {
     g_error("Non riesco ad allocare teseo_strokes_sac!");
    exit(1);
 }
 else {
 teseo_strokes_sac[lstrokes] = CANARY;
 }

 for (i=0; i<num_punti; i++) {
      teseo_strokes_sac[i] = (float) (height - vet_punti[i*2+1] - 1) ; //cambio riferimento
      media +=  teseo_strokes_sac[i]  ;
 }

 media=media / (float) num_punti;

 for (i=0; i<num_punti; i++) {
 	teseo_strokes_sac[i] = ( (teseo_strokes_sac[i] - media) * 2.54 / dpi );
 }

 for (i=0;i<num_punti; i++ ){
  g_message("teseo_strokes_sac[%d] : %f", i, teseo_strokes_sac[i]);
 }

 //da rimettere quando se ne trova la sintassi
 lun_name=strlen ( file_sac ) ;

 //31/05 g_message("media = %f, file_sac = %s, teseo_strokes_sac = %f,  max = %d ,  inizio = %f,  passo_delta = %f,  nerr = %d,  lun_name = %d", media, file_sac , teseo_strokes_sac[0] ,  max ,  inizio ,  passo_delta ,  nerr,  lun_name  );
 //wsac1( file_sac , teseo_strokes_sac ,  &max ,  &inizio ,  &passo_delta ,  &nerr,  lun_name );
 //31/05 g_message("file_sac = %s, teseo_strokes_sac = %f,  max = %d ,  inizio = %f,  passo_delta = %f,  nerr = %d,  lun_name = %d", file_sac , teseo_strokes_sac[0] ,  max ,  inizio ,   passo_delta , nerr,  lun_name  );

 //newhdr();
 if (teseo_strokes_sac[lstrokes] != CANARY ){
  //31/05 g_error("Hai ucciso il canarino in teseo_strokes_sac");
 }

}

/*TODO test !!!*/
void teseo_strokes_sac(gint32 g_image, char * file_sac, gdouble* strokes , glong num_stroke){
  FILE*  fstrokes;
  gdouble xres;
  gdouble yres;
  gint image_height;
  gimp_image_get_resolution (g_image, &xres, &yres);
  image_height = gimp_image_height(g_image);
  fstrokes = fopen(file_sac,"wt");
  if(!fstrokes)
    g_message("Unable to open%s",file_sac);
  else {
    teseo_save_sac_cm( file_sac, num_stroke, strokes, xres, image_height );
    fclose(fstrokes);
  }
}

/*SVG section*/
gboolean teseo_import_svg_vectors ( gint32 g_image, char * SVGfile  )
{
	gboolean ok ;
	gboolean merge=FALSE;

    ok=gimp_path_import(g_image, SVGfile, merge, FALSE);

    if (ok == FALSE)
    {
      /* Report error to user*/
      // fprintf (stderr, "Unable to write file: \n");
      g_message("teseo_import_svg_vectors(): unable to import file \"%s\". SVG format is not correct.", SVGfile);
    }
    return ok;
}

gboolean teseo_export_svg_vectors ( gint32 g_image, char * SVGfile  )
{
    // TODO
    return TRUE;
}


void teseo_import_svg_vectors_combining_more_components(gint32 image_ID, const gchar *filename) {
#define SIZELINE 1024
#define  POS_C  11
#define  POS_M  60
#define  PATHIDLEN  12
	FILE *f;
	FILE *fout;
	gchar line[SIZELINE];
	const gchar pathidline[PATHIDLEN] = "  <path id=";
	const gchar firstline[POS_C+2] = "           C";
	gboolean after_first_line = FALSE;
	gchar filenameout[SIZELINE];
	g_snprintf(filenameout, SIZELINE-1, "%s.combined.by.teseo2.svg", filename);
	
	f = fopen(filename, "rt");
	if(f) {
		fout = fopen(filenameout, "wt");
		if(fout) {
			while(fgets (line, SIZELINE,  f)) {
				if(after_first_line) {
					if( g_strncasecmp(line, firstline, POS_C + 1) == 0 ) {
						line[POS_C] = ' ';
					}
					if(line[POS_M] == 'M') {
						line[POS_M] = '\n';
						line[POS_M + 1] = 0;
					}
				} else {
					// check for path id
					if( g_strncasecmp(line, pathidline, PATHIDLEN-1) == 0 ) {
						// remove last character '"'
						gint line_length=strlen(line);
						line[line_length-2] = '_';
						line[line_length-1] = 'c';
						line[line_length+0] = 'o';
						line[line_length+1] = 'm';
						line[line_length+2] = 'b';
						line[line_length+3] = '_';
						line[line_length+4] = 't';
						line[line_length+5] = 'e';
						line[line_length+6] = 's';
						line[line_length+7] = 'e';
						line[line_length+8] = 'o';
						line[line_length+9] = '"';
						line[line_length+10] = '\n';
						line[line_length+11] = 0;

					// check for first coordinate
					} else if( g_strncasecmp(line, firstline, POS_C + 1) == 0 ) {
						after_first_line = TRUE;
						if(line[POS_M] == 'M') {
							line[POS_M] = '\n';
							line[POS_M + 1] = 0;
						}
					}
				}
				g_fprintf(fout, "%s", line);
			}
			fclose(fout);
			teseo_import_svg_vectors(image_ID, filenameout );
		} else {
			g_message("teseo_path_import_combining_more_components(): %s can not be created!", filenameout);
		}
		fclose(f);
	} else {
		g_message("teseo_path_import_combining_more_components(): %s does not exist!", filename);
	}
}
