#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "teseo_io.h"
#include "teseo_path.h"
#include <locale.h>

//31/05#include "neuronutils.h"
//TODO spostare o modificare salva_sisma_cm salva_dxf_cm salva_marcatempi_cm


int my_system(char * command, char * const argv[200], char * const env[]  ){
 int status = 0;
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
 return status;
}


gint fileesiste( char *nome_file)
{
  FILE *test = fopen( nome_file, "rb" );
  int app=0;
  if( test )
  	{
  		fclose(test);
  		app = 1;
  	}
  	else
  		app = 0;
  //g_message("file %s esiste %d",nome_file ,app);
  return app;
}


gint test_dir( char *nome_dir)
{
  gint ret=0;
  DIR * directory=NULL;
  if ( fileesiste( nome_dir )) {
   directory=opendir(nome_dir);
   if ( directory ){
    ret=1;
   	closedir(directory);
   }
  }
  return ret;
}


void Carica_Dxf ( gint32 g_image, char * NomeFileDxf  )
{

FILE *fp = NULL;
gchar str[20];
gchar n_punti_str[10];
gint n_punti;
gint n_punti_tot=0;
gint apro_path=0;

gint strl=0;
gint counter=0;

gint i=0, a=0;

gdouble xres;
gdouble yres;
gdouble dpi;
gint image_height;
gchar x_c[10],y_c[10];
gchar *end = NULL;

gdouble x,y;
gdouble *v_punti = NULL;

float my_x;
float my_y;

gdouble vertex[200];
gint n_vertex=1;


vertex[0]=0.0;   //dovuto alle coordinate negative presenti nel DXF
vertex[1]=0.0;


gint test_vertex=0;
char nome_path [] = "Imported Path" ;
char jpolyline[2];


setlocale(LC_NUMERIC, "C");

gimp_image_get_resolution (g_image, &xres, &yres);
//g_message("Risoluzione x=%f y=%f", xres, yres);

image_height = gimp_image_height(g_image);
//g_message("altezza y=%d", image_height);

dpi = yres;

fp = fopen( NomeFileDxf,"r" );
	if(!fp)
 		g_message("File \"%s\" not found.", NomeFileDxf);
 		else
 			{
 			 	while(!feof(fp))
				{
 			 		fscanf(fp,"%s\n",str);
 			 		if(!strncmp(str,"AcDbPolyline",12) && fscanf(fp,"%s\n",jpolyline) && !strncmp(jpolyline,"90",2))
 			 			{
					 		fscanf(fp,"%s\n70\n",n_punti_str);
 			 				n_punti_tot = atoi(n_punti_str);
 			 			}
					  else
							if( !strncmp( str, "AcDb2dVertex", 12 )  )
      			  	{
                 n_punti_tot++;
                }
       	}
				fclose(fp);
			}

v_punti = (gdouble *) malloc( ( sizeof(gdouble) ) * 2 *( n_punti_tot ));


fp = fopen( NomeFileDxf,"r" );
	if(!fp)
 		g_message("File \"%s\" not found.", NomeFileDxf);
 		else
 			{
 			 while(!feof(fp))
				{
 			 		fscanf(fp,"%s\n",str);
 			 		if(( !strncmp(str,"AcDbPolyline",12)) && (fscanf(fp,"%s\n",jpolyline)) && (!strncmp(jpolyline,"90",2)) )
 			 			{
					 		a=1;

					 		fscanf(fp,"%s\n70\n",n_punti_str);
 			 				n_punti = atoi(n_punti_str);

   					          	fscanf(fp,"0\n");

							for(i=counter; i < n_punti ;i++, counter++ )
             							{

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
 					}//chiude IF
     	 			  	else
					if( !strncmp( str, "AcDb2dVertex", 12 )  )
      			  			{
	     						a=1;
	     						test_vertex=1;
                				{

/*						fscanf(fp," 10\n");
                				fscanf(fp,"%s\n",x_c );
                				fscanf(fp," 20\n" );
	        		       		fscanf(fp,"%s\n",y_c );

                                                //x = strtod( x_c, &end);//Necessario non si sa perchè
		  		           	x = strtod( x_c, &end);
      	         				y = strtod( y_c, &end);*/


						fscanf(fp,"10\n");
						fscanf(fp,"%f\n", &my_x );
						fscanf(fp,"20\n" );
						fscanf(fp,"%f\n",&my_y );
	//					printf("X=%f Y=%f \n",my_x, my_y );

						x=my_x;
						y=my_y;


						v_punti[counter*2] = ( x/2.54*dpi );
						v_punti[counter*2+1] = ( image_height - ( y*dpi/2.54 + 1 ) );

                	  			if(v_punti[counter*2]<0)
                	  			v_punti[counter*2]=v_punti[(counter-1)*2];

                	  			counter++;

                				}
        			  	} //fine else
 			  }//chiude WHILE

 			  if(!a)
 			  	g_message("File \"%s\" corrupted.", NomeFileDxf);
 			}

strokes_to_open_path( g_image, (glong) n_punti_tot, v_punti, "Imported Path");

if(v_punti)
	free(v_punti);

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


void Carica_Bzr ( gint32 g_image, char * NomeFileBzr  )
{
 //g_message("%s",NomeFileBzr);
 FILE *fp = NULL;
 glong num_path, num_points, closed, draw, state, tipo,X,Y;
 gdouble *path = NULL;
 gchar pathname [200] = "";
 gchar * app;
 gchar linename [200] = "";
 int i=0;

 if( fp = fopen(NomeFileBzr, "r" ))
 {
  //fscanf(fp, "Name: %s\n", &linename);
	fgets(linename, 200, fp);
	//Salto Name:
	app=linename+6;
	strcpy(pathname,app);

	/*Controllo che non ci sia già un path con lo stesso nome*/

	fscanf(fp,"#POINTS: %d\n" , &num_points);
	fscanf(fp,"CLOSED: %d\n" , &closed);
	fscanf(fp,"DRAW: %d\n", &draw);
	fscanf(fp,"STATE:%d\n", &state);
	//g_message("nome=%s num_points=%d closed=%d draw=%d state=%d", pathname, num_points, closed, draw, state);

	if ( closed==0 && draw==0 && state==2 )
	{
	  //g_message("Path corretto");
  	 path = (gdouble *) malloc( num_points * 3 * sizeof(gdouble) );
 	 if (path != NULL) {
  	  for ( i=0; i<num_points; i++)    {
           fscanf(fp,"TYPE: %ld X: %ld Y: %ld\n", &tipo, &X, &Y);
           path[i*3] = ( double ) X;
           path[i*3+1] = ( double ) Y;
           path[i*3+2] = ( double ) tipo;
  	  }
     //  	  g_message("ancora ok");
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
  	free(path);
  }

  fclose(fp);
 }
 else
 {
  g_message("File \"%s\" not found.", NomeFileBzr);
 }
}



/*
void Carica_Timemarker ( gint32 g_image, char * NomeFileTimeMarker ){
	g_message("Pazienza %s non lo so leggere", NomeFileTimeMarker );
}
*/

/*Funzioni di output*/

gint open_path_to_save(gint32 g_image,  char * nome_path, char * filename){
  glong i=0, num_righe, len;
  gdouble * points_pairs=NULL;
  gdouble * pstrokes_ret=NULL;
  FILE * fp=NULL;
  gint path_type, path_closed, num_path_point_details;
  char s_tmp_app[255];
  char * canc=NULL;
  strcpy(s_tmp_app,nome_path);

  //ottengo il path
	gimp_path_get_points (g_image, nome_path, &path_closed, &num_path_point_details, &points_pairs);
  //se il path è corretto lo salvo
  if (!path_closed){
     //+3 perchè per il primo punto scrive sei details, per gli altri nove ( x, y, tipo )
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
      fprintf(fp,"#POINTS: %d\nCLOSED: 0\nDRAW: 0\nSTATE:2\n",num_righe);
     	for (i=0; i<num_righe; i++)    {
         fprintf(fp,"TYPE: %d X: %d Y: %d\n",(glong) floor(points_pairs[i*3+2]), (glong) floor(points_pairs[i*3]),(glong)   floor(points_pairs[i*3+1]));
     	}
      fclose(fp);
     }

  }
return 1;
}



//gint salva_path(gpointer user_data ){
// //g_message("passa il tempo");
// gint num_paths=0;
// gimp_path_list (g_image_gen, &num_paths);
// if (num_paths>0){
// //g_message("%s", ( char * ) user_data);
// open_path_to_save(g_image_gen, gimp_path_get_current(g_image_gen), (char *) user_data);
// }
//return 1;
//}


/*Da fare : introdurre un parametro in più per unificare le due di sotto*/

void salva_path_dxf(gint32 g_image, char* filename, gint scalatura){
 //recupero il path in uno strokes, poi salva in dxf
 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
 /*Se esiste una traccia esegue*/
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = open_path_to_strokes(g_image, &n_strokes, pathname);
   /*campiono se necessario*/
   strokes_dxf(g_image,filename, strokes, n_strokes, 1, scalatura); //tracciato
 }
 if (strokes)
  free(strokes);
}


void	salva_path_timemarker(gint32 g_image, char* filename, gint scalatura){
 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
 /*Se esiste una traccia esegue*/
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = open_path_to_strokes(g_image, &n_strokes, pathname);
   strokes_dxf(g_image, filename, strokes, n_strokes, 0, scalatura ); //0 sta per salva marcatempi
 }
 if (strokes)
  free(strokes);

}

void salva_path_sisma(gint32 g_image, char* filename){
 //recupero il path in uno strokes, poi salva in dxf
 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 gint num_paths=0;
 /*Se esiste una traccia esegue*/
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = open_path_to_strokes(g_image, &n_strokes, pathname);
//31/05   strokes_sisma(g_image, filename, strokes, n_strokes);
   strokes_sisma(g_image, filename, strokes, n_strokes, 0);
 }
 if (strokes)
  free(strokes);
}

/* initialized_old
void strokes_dxf(gint32 g_image, const char * file_dxf, gdouble* strokes , glong num_stroke, gint tracciato=1 , gint scalatura=0){
*/
void strokes_dxf(gint32 g_image, const char * file_dxf, gdouble* strokes , glong num_stroke, gint tracciato, gint scalatura){

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
      	//TODO salva_dxf_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
	salva_dxf_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
      }
      else
      { //se salvo un marcatempo
     		//TODO salva_marcatempi_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
		salva_marcatempi_cm(fstrokes, num_stroke, strokes, xres, image_height, scalatura);
      }
     }
     else
      {
      	// g_message("Non so proprio cosa salvare");
      }

     fclose(fstrokes);

  }
}

/* initialized_old
void strokes_sisma(gint32 g_image, const char * file_sisma, gdouble* strokes , glong num_stroke, gchar xy=0){
*/
void strokes_sisma(gint32 g_image, const char * file_sisma, gdouble* strokes , glong num_stroke, gchar xy){
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
    //TODO salva_sisma_cm(fstrokes,num_stroke, strokes, xres, image_height, xy );
    salva_sisma_cm(fstrokes,num_stroke, strokes, xres, image_height, xy );
    fclose(fstrokes);
  }
}

/*
void salva_path_traccia(gint32 g_image, char* filename){
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
    strokes = open_path_to_strokes(g_image, & n_strokes,  pathname);
    //traduco lo strokes in oggetto punti
    punti = new	neuron_punti(strokes, n_strokes);

    strcpy(nome_immagine,gimp_image_get_filename(g_image));
    punti->writefiletraccia(filename, nome_immagine, "Creato con Neuron Teseo");
    if (punti)
     delete punti;
  }
}
*/

/* Da usare sul path campionato a passo 1*/
void salva_path_sac(gint32 g_image, char* filename, char *dir_teseo_bin, gchar xy ){
 //recupero il path in uno strokes, poi salva in dxf

 gdouble * strokes=NULL;
 glong n_strokes;
 char pathname [80] ;
 char strcmd [200];

 char *argv[6];
 char argv0[100];
 char argv1[100];
 char argv2[100];
 char argv3[100];
 char argv4[100];

 char *env[]={"SACAUX=/",NULL}; //Vuole SACAUX anche se non usata

 char filename_tmp [255];

//31/05  sprintf(filename_tmp, "%s/tmp/sisma.tmp", getenv_teseo(TESEO_DATA));
 //TODO definire l'ambiente o prendere getenv_teseo e neuron_parse da teseo1
 sprintf(filename_tmp, "%s/tmp/sisma.tmp", "/" );
 gint num_paths=0;
 gdouble xres, yres, passo;
 gint image_height;

 gimp_image_get_resolution (g_image, &xres, &yres);
 passo=2.54/xres;

 /*Se esiste una traccia esegue*/
 gimp_path_list (g_image, &num_paths);
 if (num_paths>0){
   //prendo il nome del path corrente
   strcpy(pathname, gimp_path_get_current(g_image));
   strokes = open_path_to_strokes(g_image, &n_strokes, pathname);
   //strokes_sac(filename, strokes, n_strokes);
   //scrivo file sisma temporaneo
   strokes_sisma(g_image, filename_tmp, strokes, n_strokes, xy);
   //converto il sisma temporaneo in sac
   sprintf(strcmd, "%s/bin/sisma_to_sac.exe" , dir_teseo_bin );

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

 	 /*Chiamata al driver sisma_to_sac*/
 	 my_system (strcmd,argv,env);

 }
 if (strokes)
  free(strokes);
}



void Carica_Timemarker ( gint32 g_image, char * NomeFileTimeMarker )
{
 //g_message("%s",NomeFileTimeMarker);
 FILE *fp = NULL;
 glong num_path, closed, draw, state, tipo,X,Y;
 gdouble *strokes = NULL;
 gchar pathname [200] = "";
 gchar * app;
 gchar linename [200] = "";
 gdouble xres,yres;
 gdouble dpi;
 gint image_height;
 int a=0,b=0,c=0;
 long counter=0;
 long counter_void;
 long counter0 = 32-1;
 double h_seg=10.0;
 double x_zero=0.0;
 double y_zero=0.0;
 double x1,y1,x2,y2;

 int i=0;

 if( fp = fopen(NomeFileTimeMarker, "r" ))
 {
	/*Intestazione*/

	fscanf(fp,"%d\nSECTION\n%d\nENTITIES\n%d\n", &a, &b, &c);

	if ( a==0 && b==2 && c==0 )
	{

	  while( fscanf(fp,"LINE\n5\n%ld\n6\nContinuous\n62\n3\n100\nAcDbLine\n10\n%lf\n20\n%lf\n30\n0.0\n11\n%lf\n21\n%lf\n31\n0.0\n0\n", &counter, &x1, &y1, &x2, &y2 ) == 5 ){
	  	if ( counter == 32 ) y_zero=y1;
	  }

  	fclose(fp);
  	fp = fopen(NomeFileTimeMarker, "r");
	  counter = counter-counter0;

	  fscanf(fp,"%d\nSECTION\n%d\nENTITIES\n%d\n", &a, &b, &c);

   	strokes = (gdouble *) malloc( counter * 2 * sizeof(gdouble) );
   	if (strokes != NULL) {
   	  gimp_image_get_resolution (g_image, &xres,&yres);
   	  dpi=xres;
   	  image_height = gimp_image_height(g_image);

   	  for ( i=0; i<counter; i++)    {

           fscanf(fp,"LINE\n5\n%ld\n6\nContinuous\n62\n3\n100\nAcDbLine\n10\n%lf\n20\n%lf\n30\n0.0\n11\n%lf\n21\n%lf\n31\n0.0\n0\n", &counter_void, &x1, &y1, &x2, &y2 );
           //x=(vet_punti[i*2]-x_zero) * 2.54 / dpi;
           strokes[i*2] = x1*dpi/2.54;
           //y=(height - vet_punti[i*2+1] - 1 - y_zero) * 2.54/dpi;  ... y_zero = height - vet_punti[1] - 1
           strokes[i*2+1] = image_height - y1 * dpi / 2.54 - 1;
           //g_message("X=%lf Y=%lf", strokes[i*2], strokes[i*2+1] );
   	  }

       strokes_to_open_path( g_image, (glong) counter, strokes, "Imported Path");

  	  }
    else {
  	 	g_message("Not enough free memory for timemark path.");
    }
	}
 	else {
 		g_message("File corrupted.");
 	}

 	if(strokes) {
  	free(strokes);
  }

  fclose(fp);
 }
 else
 {
  g_message("File \"%s\" not found.", NomeFileTimeMarker);
 }
}



/*NEW entries*/

void salva_dxf(FILE * fp, gint num_punti, gdouble *vet_punti){

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
void salva_dxf_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start=1){
*/
void salva_dxf_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start){
double x_zero=0.0;
double y_zero=0.0;
//double x,y;

long i=0;

if( zero_start ) {
	x_zero=vet_punti[0];
	y_zero= height - vet_punti[1] - 1 ;
}

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

fprintf(fp,"90\n%d\n70\n", num_punti);   //numero punti

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
);  // chiudo il file

}

/* initialized_old
void salva_marcatempi_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start=1){
*/
void salva_marcatempi_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar zero_start){

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

fprintf(fp,
"0\n\
SECTION\n\
2\n\
ENTITIES\n\
0\n"
);

//intestazione

for (i=0; i<num_punti; i++) {
//scrivo le sezioni line

x=(vet_punti[i*2]-x_zero) * 2.54 / dpi;
y=(height - vet_punti[i*2+1] - 1 - y_zero) * 2.54/dpi;

fprintf(fp,
"LINE\n\
5\n\
%d\n\
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
);  // chiudo il file
}

//Salvataggio in formato sisma
//dpi risoluzione in DPI, height altezza immagine in pixel
/* initialized_old
void salva_sisma_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar xy=0){
*/
void salva_sisma_cm(FILE * fp, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height, gchar xy){

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

//extern void  wsac1 ( char * kname , float * strokes_sac , int * max , float * inizio , float * passo_delta , int * nerr , int *kname_num ) ;


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
/*void salva_sac_cm( char * file_sac, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height){
  long int max = 100 , j , nerr=0 ;
  float yfunc[ max ] , x , beg = 0. , del = 0.02 ;
  for ( j = 0 , x = beg ; j < max ; j++ , x += del )
        yfunc[ j ] = exp ( -x ) ;
  newhdr();
  wsac1 ( file_sac , yfunc , &max , &beg , &del , &nerr , strlen ( file_sac ) ) ;
  g_message("errore : %d",nerr);
}
*/

/*Probabilmente non funziona !!!*/
void salva_sac_cm( char * file_sac, gint num_punti, gdouble *vet_punti, gdouble dpi, gint height){

 float media=0;
 float * strokes_sac;
 float CANARY=-123.456789e6;
 long int max = (long int ) num_punti;
 long int j , nerr=0, lstrokes, lun_name;
 float x , inizio = vet_punti[0]*2.54/dpi , passo_delta = 2.54/dpi ;
 int i=0;

 g_message("num_punti %d, dpi %f, height %d ", num_punti, dpi, height);
 for (i=0;i<num_punti*2; i++ ){
  g_message("vet_punti[%d] : %f", i, vet_punti[i]);
 }

 lstrokes=num_punti;
 strokes_sac = (float *) malloc(sizeof(float) * ( lstrokes + 1 ));
 if(!strokes_sac) {
     g_error("Non riesco ad allocare strokes_sac!");
    exit(1);
 }
 else {
 strokes_sac[lstrokes] = CANARY;
 }

 for (i=0; i<num_punti; i++) {
      strokes_sac[i] = (float) (height - vet_punti[i*2+1] - 1) ; //cambio riferimento
      media +=  strokes_sac[i]  ;
 }

 media=media / (float) num_punti;

 for (i=0; i<num_punti; i++) {
 	strokes_sac[i] = ( (strokes_sac[i] - media) * 2.54 / dpi );
 }

 for (i=0;i<num_punti; i++ ){
  g_message("strokes_sac[%d] : %f", i, strokes_sac[i]);
 }

 //da rimettere quando se ne trova la sintassi
 lun_name=strlen ( file_sac ) ;

 //31/05 g_message("media = %f, file_sac = %s, strokes_sac = %f,  max = %d ,  inizio = %f,  passo_delta = %f,  nerr = %d,  lun_name = %d", media, file_sac , strokes_sac[0] ,  max ,  inizio ,  passo_delta ,  nerr,  lun_name  );
 //wsac1( file_sac , strokes_sac ,  &max ,  &inizio ,  &passo_delta ,  &nerr,  lun_name );
 //31/05 g_message("file_sac = %s, strokes_sac = %f,  max = %d ,  inizio = %f,  passo_delta = %f,  nerr = %d,  lun_name = %d", file_sac , strokes_sac[0] ,  max ,  inizio ,   passo_delta , nerr,  lun_name  );

 //newhdr();
 if (strokes_sac[lstrokes] != CANARY ){
  //31/05 g_error("Hai ucciso il canarino in strokes_sac");
 }

}

/*Probabilmente non funziona !!!*/
void strokes_sac(gint32 g_image, char * file_sac, gdouble* strokes , glong num_stroke){
  FILE*  fstrokes;
  gdouble xres;
  gdouble yres;
  gint image_height;
  gimp_image_get_resolution (g_image, &xres, &yres);
  image_height = gimp_image_height(g_image);
  fstrokes = fopen(file_sac,"wt");
  if(!fstrokes)
    g_message("Non riesco ad aprire %s",file_sac);
  else {
    salva_sac_cm( file_sac, num_stroke, strokes, xres, image_height );
    fclose(fstrokes);
  }
}
