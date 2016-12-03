#ifndef _NA_ROUTINE_H
#define _NA_ROUTINE_H

#include "na_figure.h"

int na_check_fig(struct na_figure *fig);
struct na_figure *na_read_figs(FILE *file, int *nfigs);
struct na_figure *na_makeset(struct na_figure *figs, int nfigs, int *setsize);
int na_copy_fig(struct na_figure *dest, struct na_figure *src);
void na_destroy_fig(struct na_figure *fig);
int na_calc_gc(struct na_figure *fig);

#endif

