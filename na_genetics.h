#ifndef _NA_GENETICS_H
#define _NA_GENETICS_H

#include "na_figure.h"

struct na_individ {
	int genom_size, *genom;
	double fitness;
};

int na_compare_individs(struct na_individ *indiv1, struct na_individ *indiv2, struct na_figure *figset);
int na_crossover(struct na_individ *indiv1, struct na_individ *indiv2);
struct na_individ *na_mutate(struct na_individ *indiv);
int na_check_individ(struct na_individ *indiv);

#endif

