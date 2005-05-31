#ifndef NEURONENV
#define NEURONENV 1


extern const char TESEO_BIN[];
extern const char TESEO_DATA[];

char *valore_parse(const char *nomefile, const char *nomevariabile);
char *getenv_teseo(const char *name_var) ;
#endif
