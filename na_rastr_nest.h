#ifndef _NA_MTXNEST_H
#define _NA_MTXNEST_H

#define NA_CHECKPOS_HEIGHT 0x0
#define NA_CHECKPOS_SCALE 0x2
#define NA_POSITION_FAST 0x0
#define NA_POSITION_FULL 0x1

#include "na_figure.h"
#include "na_nest_structs.h"
#include "na_genetics.h"

int na_rastrnest(struct na_figure *figset, int setsize, 
				 struct na_individ *indiv, int w, int h, 
				 struct na_nestattrs *attrs);

#endif

