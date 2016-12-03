#ifndef _NA_RASTR_NEST_H
#define _NA_RASTR_NEST_H

#include "na_rastr.h"

enum NA_CHECK_POSITION_MODE {
	NA_CHECK_POSITION_MODE_HEIGHT,
	NA_CHECK_POSITION_MODE_SCALE
};

enum NA_PLACEMENT_MODE {
	NA_PLACEMENT_MODE_FAST,
	NA_PLACEMENT_MODE_FULL
};

#include "na_figure.h"
#include "na_genetics.h"

struct na_nest_attrs {
	int bound, resize, flags;
	enum NA_CHECK_POSITION_MODE check_position_mode;
	enum NA_PLACEMENT_MODE placement_mode;
	enum NA_RASTR_TYPE rastr_type;
};

struct na_position {
	struct na_figure *fig;
	double x, y;
	int angle;
};


int na_rastr_nest(struct na_figure *figset, int set_size,
	  	  	  	  struct na_individ *indiv, int width,
				  int height, struct na_nest_attrs *attrs);

#endif

