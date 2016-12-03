#ifndef _NA_CHECKPOS_H
#define _NA_CHECKPOS_H

#include "na_figure.h"

int checkpos_height(struct na_figure *fig, struct na_position *posits,
					int npos, int xpos, int ypos, int w, int h, 
					int *placed);
int checkpos_scale(struct na_figure *fig, struct na_position *posits,
				   int npos, int xpos, int ypos, int w, int h, 
				   int *placed);

#endif

