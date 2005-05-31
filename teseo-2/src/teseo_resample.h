#ifndef TESEO_RESAMPLEH
#define TESEO_RESAMPLEH 1

#define GIMP_ENABLE_COMPAT_CRUFT 1
#include <libgimp/gimp.h>
#include <libgimp/gimpcompat.h>
#include "teseo_types.h"


void sottocampiona_strokes(double *strokes, glong *pn_strokes, int passo);
void campionamento_progressivo_strokes(double *strokes, glong *pn_strokes);
// void call_on_btnBezier_clicked (gint32 g_image_gen, NeuronSismosVals pivals);
void call_on_btnBezier_clicked (gint32 g_image_gen,  int sw_campionamento_progressivo, gint passo_bezier);

#endif
