#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "na_check_position.h"
#include "na_errno.h"
#include "na_figure.h"
#include "na_genetics.h"
#include "na_rastr.h"
#include "na_rastr_nest.h"
#include "na_transform.h"

#include "na_check_position.h"
#include "na_routine.h"
#include "na_transform.h"

static int place_fig_fast(struct na_figure *fig, struct na_position *posits,
		   	   	   	   	  int npos, int width, int height, int resize,
						  int bound, int **place);

enum NA_RASTR_TYPE rastr_type;
enum NA_CHECK_POSITION_MODE check_position_mode;

static int
place_fig_fast(struct na_figure *fig, struct na_position *posits,
			   int npos, int width, int height, int resize,
			   int bound, int **place)
{
	int i, j, x, y, placed = 0, angle, angstep;
	struct na_figure *curr_fig;
	struct na_rastr *rastr;

	angstep = (fig->angstep == 0) ? 360 : fig->angstep;

	for (angle = 0; angle < 360; angle += angstep) {
		if ((curr_fig = malloc(sizeof(struct na_figure))) == NULL) {
			na_errno = NA_ENOMEM;
			return -1;
		}
		if (na_copy_fig(curr_fig, fig) < 0) {
			free(curr_fig);
			return -1;
		}

		na_rotate(fig, angle);

		if ((rastr = na_fig_to_rastr(fig, rastr_type, resize, bound)) == NULL) {
			na_destroy_fig(curr_fig);
			return -1;
		}

		if (rastr->width > width / resize || rastr->height > height / resize)
			continue;

		for (y = 0; y < height / resize - rastr->height; y++) {
			for (x = 0; x < width / resize - rastr->width; x++) {
				int flag = 0;
				int k;
				struct na_point *pts;

				for (k = 0, pts = rastr->outer_contour; k < rastr->npts; k++, pts++) {
					i = pts->y;
					j = pts->x;

					if (place[y + i][x + j] > 0) {
						flag = 1;
						break;
					}

					if (flag > 0)
						break;
				}
				if (flag > 0)
					continue;

				if (na_check_position_height(fig, posits, npos, x * resize,
											 y * resize, width,
											height, &placed) == 0) {
					posits[npos].angle = angle;
				} else if (na_errno != NA_SUCCESS) {
					if (placed == 1)
						na_destroy_fig(posits[npos].fig);
						free(posits[npos].fig);
						return -1;
					}

				x = width;
				y = height;
			}
		}
		na_destroy_fig(curr_fig);
		na_destroy_rastr(rastr);
	}

	if (placed == 0)
		return -1;

	if ((rastr = na_fig_to_rastr(posits[npos].fig, rastr_type, resize, bound)) == NULL) {
		na_destroy_fig(posits[npos].fig);
		free(posits[npos].fig);
	}

	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++) {
			int x_place, y_place;

			x_place = posits[npos].x / resize;
			y_place = posits[npos].y / resize;

			place[i + y_place][j + x_place] += rastr->rastr_matrix[i][j];
		}

	na_destroy_rastr(rastr);

	return 0;
}

int
na_rastr_nest(struct na_figure *fig_set, int set_size,
			  struct na_individ *indiv, int width,
			  int height, struct na_nest_attrs *attrs)
{
	int i, j, npos, resize, bound, *mask, **place;
	struct na_position *posits;
	FILE *tmp;

	if (set_size == 0 || width <= 0 || height <= 0 ||
		indiv == NULL || fig_set == NULL) {
		na_errno = NA_EINVAL;
		goto fail_exit;
	}

	for (i = 0; i < set_size; i++)
		if (na_check_fig(fig_set + i) < 0) {
			na_errno = NA_EINVAL;
			goto fail_exit;
		}

	if (indiv->genom_size > 0 && indiv->genom == NULL) {
		na_errno = NA_EINVAL;
		goto fail_exit;
	}

	resize = 1;
	bound = 0;
	if (attrs != NULL) {
		resize = (attrs->resize <= 0) ? 1 : attrs->resize;
		bound = attrs->bound;
		rastr_type = attrs->rastr_type;
	}

	if ((posits = malloc(sizeof(struct na_position) * set_size)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}

	if ((mask = calloc(set_size, sizeof(int))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_posits;
	}

	if ((place = malloc(sizeof(int *) * height / resize)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_mask;
	}

	for (i = 0; i < height / resize; i++)
		if ((place[i] = calloc(width / resize, sizeof(int))) == NULL) {
			na_errno = NA_ENOMEM;
			i--;
			goto fail_free_place;
		}

	if ((posits[0].fig = malloc(sizeof(struct na_figure))) == NULL) {
		na_errno = NA_ENOMEM;
		free(posits[0].fig);
		goto fail_free_positions;
	}

	if (indiv->genom_size == 0)
		if ((indiv->genom = malloc(sizeof(int) * set_size)) == NULL) {
			na_errno = NA_ENOMEM;
			goto fail_free_positions;
		}

	//place_height = 0.0;
	npos = 0;
	for (i = 0; i < indiv->genom_size; i++) {
		int fig_num;
		struct na_figure *fig;

		fig_num = indiv->genom[i];
		fig = fig_set + fig_num;
		na_errno = NA_SUCCESS;

		if (place_fig_fast(fig, posits, npos, width, height, resize, bound, place) == 0) {
			printf("positioned %d x=%lf y=%lf\n", fig_num, posits[npos].x, posits[npos].y);
			na_translate(fig, posits[npos].x, posits[npos].y);
			npos++;
			if (npos < set_size)
				if ((posits[npos].fig = malloc(sizeof(struct na_figure))) == NULL) {
					na_errno = NA_ENOMEM;
					free(posits[npos].fig);
					goto fail_free_genom;
				}

			mask[fig_num] = 1;
		} else if (na_errno != NA_SUCCESS) {
			i = npos - 1;
			goto fail_free_genom;
		}
	}

	if (npos < indiv->genom_size) {
		indiv->fitness = INFINITY;
		indiv->posits = NULL;
		i = npos - 1;
		goto fail_free_positions;
	}

	for (i = 0; i < set_size; i++) {
			int fig_num;
			struct na_figure *fig;

			if (mask[i] > 0)
				continue;

			fig_num = i;
			fig = fig_set + fig_num;
			na_errno = NA_SUCCESS;

			if (place_fig_fast(fig, posits, npos, width, height, resize, bound, place) == 0) {
				printf("positioned %d x=%lf y=%lf\n", fig_num, posits[npos].x, posits[npos].y);
				na_translate(fig, posits[npos].x, posits[npos].y);
				npos++;
				if (npos < set_size)
					if ((posits[npos].fig = malloc(sizeof(struct na_figure))) == NULL) {
						na_errno = NA_ENOMEM;
						free(posits[npos].fig);
						goto fail_free_genom;
					}
				mask[fig_num] = 1;
			} else if (na_errno != NA_SUCCESS) {
				i = npos - 1;
				goto fail_free_genom;
			}
		}

	tmp = fopen("/home/vadim/SvgFiles/place", "w+");

	for (i = 0; i < height / resize; i++) {
		for (j = 0; j < width / resize; j++)
			fprintf(tmp, "%d", place[i][j]);
		fprintf(tmp, "\n");
	}

	indiv->genom_size = npos;
	indiv->posits = posits;

	return 0;

fail_free_genom:
	free(indiv->genom);

fail_free_positions:
	for (; i >= 0; i--) {
		na_destroy_fig(posits[i].fig);
		free(posits[i].fig);
	}
	free(posits);

fail_free_place:
	for (; i >= 0; i--)
		free(place[i]);
	free(place);

fail_free_mask:
	free(mask);

fail_free_posits:
	free(posits);

fail_exit:
	return 1;
}
