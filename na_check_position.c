#include "na_check_position.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "na_errno.h"
#include "na_routine.h"
#include "na_rastr_nest.h"

int
na_check_position_height(struct na_figure *fig, struct na_position *posits,
						 int npos, int xpos, int ypos, int width, int height,
						 int *placed)
{
	int res = -1;
	double curr_height, curr_width;

	curr_height = fig->height + ypos;
	curr_width = fig->width + xpos;

	if (curr_height >= height || curr_width >= width)
		return res;

	if (*placed == 0) {
		if ((posits[npos].fig = malloc(sizeof(struct na_figure))) == NULL) {
			na_errno = NA_ENOMEM;
			return res;
		}

		if  (na_copy_fig(posits[npos].fig, fig) < 0) {
			free(posits[npos].fig);
			return res;
		}

		posits[npos].x = xpos;
		posits[npos].y = ypos;

		*placed = 1;
		res = 0;
	} else if (*placed == 1) {
		int bool_expr_main, bool_expr_tmp;
		double prev_height, prev_width, curr_mass_center_y, prev_mass_center_y;

		prev_height = posits[npos].y + posits[npos].fig->height;
		prev_width = posits[npos].x + posits[npos].fig->width;
		prev_mass_center_y = posits[npos].y + posits[npos].fig->mass_center.y;
		curr_mass_center_y = ypos + fig->mass_center.y;

		bool_expr_main = curr_height < prev_height;

		bool_expr_tmp = fabs(curr_height - prev_height) < DBL_EPSILON &&
						curr_mass_center_y < prev_mass_center_y;
		bool_expr_main = bool_expr_main || bool_expr_tmp;

		bool_expr_tmp = fabs(curr_height - prev_height) < DBL_EPSILON &&
						fabs(curr_mass_center_y - prev_mass_center_y) < DBL_EPSILON &&
						curr_width < prev_width;
		bool_expr_main = bool_expr_main || bool_expr_tmp;

		if (bool_expr_main) {
			na_destroy_fig(posits[npos].fig);
			if (na_copy_fig(posits[npos].fig, fig) < 0) {
				free(posits[npos].fig);
				return res;
			}

			res = 0;
			posits[npos].x = xpos;
			posits[npos].y = ypos;
		}
	}

	return res;
}
