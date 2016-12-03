#ifndef _NA_TRANSFORM_H
#define _NA_TRANSFORM_H

#include "na_figure.h"

void na_move_to_zero(struct na_figure *fig);
void na_transform(struct na_figure *fig, double matrix[3][3]);
void na_rotate(struct na_figure *fig, int angle);
void na_translate(struct na_figure *fig, double a, double b);

#endif

