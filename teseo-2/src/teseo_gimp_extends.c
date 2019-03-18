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

#include <stdlib.h>
#include "teseo_gimp_extends.h"


// Wrapper to find next guide with orientation in parameter
gint32 teseo_gimp_image_find_next_guide_orientation(gint32 g_image, gint32 g_guide, gint32 g_orientation) {
    gint32 curr_guide = g_guide;
    gint32 curr_orientation;
    gboolean continue_while = TRUE;

    while(continue_while) {
	curr_guide = gimp_image_find_next_guide(g_image, curr_guide);
	if(curr_guide !=0 ) {
	    curr_orientation = gimp_image_get_guide_orientation(g_image, curr_guide);
	    if(curr_orientation == g_orientation) {
		continue_while = FALSE;
	    }
	} else {
	    continue_while = FALSE;
	}
    }

    return curr_guide;
}

int compar( const void *n1,  const void *n2 )
{
        return *((gint32 *) n1) - *((gint32 *) n2);
}

// Return a coordinates sorted list of guides with orientation in parameter. *guides need g_free
gint32 teseo_gimp_image_find_guides_orientation(gint32 g_image, gint32 g_orientation, gint32 **a_guides) {
    gint32 *guides = g_malloc(100 * sizeof(gint32)); // TODO max 100 guides (improve with g_realloc)
    gint32 n_guides = 0;
    gint32 curr_guide = 0;
    gboolean continue_while = TRUE;

    while(continue_while) {
	curr_guide = teseo_gimp_image_find_next_guide_orientation(g_image, curr_guide, g_orientation);
	if(curr_guide !=0 ) {
	    if(gimp_image_get_guide_orientation(g_image, curr_guide) == g_orientation) {
                guides[n_guides++] = gimp_image_get_guide_position(g_image, curr_guide);
	    }
	} else {
	    continue_while = FALSE;
	}
    }

    if(n_guides>0) {
        qsort(guides, n_guides, sizeof(gint32), compar ); 
    }

    *a_guides = guides;
    return n_guides;
}

void teseo_gimp_image_delete_all_guides(gint32 g_image, gint32 g_orientation) {
    gint32 curr_guide = 1;

    while(curr_guide) {
        curr_guide = teseo_gimp_image_find_next_guide_orientation(g_image, 0, g_orientation);
        if(curr_guide !=0 ) {
            gimp_image_delete_guide(g_image, curr_guide);
        }
    }

}

// Wrapper to intercept error/warning and display with g_message
gint teseo_gimp_path_get_points (gint32           image_ID,
			const gchar     *name,
			gint            *path_closed,
			gint            *num_path_point_details,
			gdouble        **points_pairs) {
    const gchar message_gimp_vector_warning[] = " Gimp-Vectors-WARNING **: gimp_vectors_compat_get_points(): convert failed";
    gint ret;
    ret = gimp_path_get_points(image_ID, name, path_closed, num_path_point_details, points_pairs);

    if(ret) {
	if(*path_closed != 0) {
	    g_message("TESEO-Warning: path \"%s\" is closed !", name);
	}
	if(*num_path_point_details < 1) {
	    g_message("TESEO-Warning: path \"%s\" is empty !", name);
	}
    } else {
	    g_message("TESEO-Warning: gimp_path_get_points() has been returning NULL for path \"%s\" !", name);
    }

    if( (!ret) && (*path_closed != 0) &&  (*num_path_point_details < 1) ) {
	    // g_message("TESEO-CRITICAL: Path contains more than one component !\nReading path \"%s\", gimp_path_get_points() maybe has printed: \"%s\"\nTake a look in gimp-2.2.6/app/vectors/gimpvectors-compat.c line 164 of 281\nTESEO OPERATION FAILED !!!", name, message_gimp_vector_warning);
	    g_message("TESEO-IMPORTANT-ADVICE: Path \"%s\" contains more than one component !\n\nIn order to combine multiple components within a path, Teseo2 offers a special SVG Import.\n1) In GIMP Paths Dialog, export in a file the path that contains more components.\n2) In Teseo menu follow \"File-->Path-->Import-->SVG_Combine\" and choose the file exported previously.", name);
    }

    return ret;
}


