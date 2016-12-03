#ifndef _NA_CHECK_POSITION_H
#define _NA_CHECK_POSITION_H

#include "na_rastr_nest.h"
#include "na_figure.h"

int na_check_position_height(struct na_figure *fig, struct na_position *posits,
							 int npos, int xpos, int ypos, int w, int h,
							 int *placed);
int checkpos_scale(struct na_figure *fig, struct na_position *posits,
				   int npos, int xpos, int ypos, int width, int height,
				   int *placed);

#endif

