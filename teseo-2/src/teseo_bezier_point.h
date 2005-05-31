#ifndef TESEO_BEZIER_POINTH
#define TESEO_BEZIER_POINTH 1

struct teseo_bezier_point {
    // grado del Bezier
    int grado;
    // n_punti sarà sempre grado+1
    int n_punti;
    // vettori lunghi n_punti contenenti le coordinate dei punti di Bezier
    double *Px, *Py;
};

    // i vettori devono essere lunghi agrado+1, dove il primo e l'ultimo valore sono di tipo ANCHOR_BEZIER
    void teseo_bezier_point_init(struct teseo_bezier_point *tbp, int agrado, double *aPx, double *aP);
    void teseo_bezier_point_free(struct teseo_bezier_point *tbp);
    void teseo_bezier_point_setPoints(struct teseo_bezier_point *tbp, double *aPx, double *aPy);
    void teseo_bezier_point_setPointsZero(struct teseo_bezier_point *tbp);
    // ritorna il numero di elementi e il vettore vet con coordinate x,y
    // campionato in x esattamente a passo freq_c
    // i tipi garantiscono compatibità con i costruttori di neuronpunti
    // si può scegliere se gli strokes ritornati abbiano un casting (int), per default è double
    int teseo_bezier_point_getStrokes(struct teseo_bezier_point *tbp, int freq_c, double **astrokes, int sw_cast_int);
    int teseo_bezier_point_Grado(struct teseo_bezier_point *tbp);

#endif
