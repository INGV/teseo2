#include <stdio.h>
#include <string.h>
#include "teseo_env.h"
#include <libgimp/gimp.h>

#define LEN_RIGA 1024


const char TESEO_BIN[] = "TESEO_BIN";
const char TESEO_DATA[] = "TESEO_DATA";


char *valore_parse(const char *nomefile, const char *nomevariabile) {
  static char ret_valore[255];
  char riga[LEN_RIGA];
  char *token;
  const char *delim = " =\t\n";
  FILE *f;
  int trovato = 0;
  ret_valore[0] = 0;
  // printf("\n");
  f = fopen(nomefile, "rt");
  if(f) {
    while(fgets(riga, LEN_RIGA-1, f) != NULL  &&  !trovato) {
      token = strtok(riga, delim);
      // printf("primo token : %s\n", token);
      if(token && strcmp(token, "#") != 0) {
        if(token && strcmp(token, nomevariabile) == 0) {
          token = strtok(NULL, delim);
          // printf("succ. token : %s\n", token);
          trovato = 1;
          if(token) {
            strcpy(ret_valore, token);
          }
        }
      }
    }
    fclose(f);
  } else {
    printf("\nNon riesco ad aprire \"%s\" !\n", nomefile);
  }
  return (ret_valore[0] == 0)? NULL : ret_valore;
}


char *getenv_teseo(const char *name_var) {
  char s_app[200];
  char *s;
  char *cmdconfig = "/usr/local/neuronteseo/bin/neuronteseo-config.sh";

  char nomefile[255];
  sprintf(nomefile, "%s/.teseorc", getenv("HOME"));
  s = valore_parse(nomefile, name_var);
  // g_message("newfunc %s = %s", name_var, s);

  // s = getenv(name_var);
  if(!s) {
    if(strcmp(name_var , TESEO_BIN)==0) {
      sprintf(s_app, "%s/teseo", getenv("HOME"));
    } else {
      sprintf(s_app, "%s", getenv("HOME"));
    }
    s = s_app;
    g_message("E' necessario impostare la variabile d'ambiente \"%s\" !\n\n- Se hai installato il pacchetto RPM, chiudi GIMP ed esegui lo script \"%s\".\n- Se invece hai compilato i sorgenti, hai dimenticato di eseguire \"make install\"\n\nLa variabile per ora viene impostata a \"%s\"", name_var, cmdconfig, s);
  } else {
    // g_message("Variabile d'ambiente %s = %s", name_var, s);
  }
  return s;
}
