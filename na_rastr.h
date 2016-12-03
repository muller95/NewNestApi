#ifndef _NA_RASTR_H
#define _NA_RASTR_H

struct na_rastr {
	int **rastr_matrix;
	int width, height, npts;
	struct na_point *outer_contour;
};

enum NA_RASTR_TYPE {
	NA_RASTR_TYPE_SIMPLE,
	NA_RASTR_TYPE_PART_IN_PART
};

#include "na_figure.h"

struct na_rastr *na_fig_to_rastr(struct na_figure *fig, enum NA_RASTR_TYPE rastr_type,
									int resize, int bound);

void na_destroy_rastr(struct na_rastr *rastr);

#endif

