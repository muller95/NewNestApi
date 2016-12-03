#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "na_errno.h"
#include "na_routine.h"
#include "na_rastr.h"

static double *get_intervals(double y1, double y2, int *nints);
static double calc_x(struct na_point *p1, struct na_point *p2, double y);
static void flood_rastr(struct na_rastr *rastr);
static void mark_contour(struct na_rastr *rastr, int i, int j);
static void find_contour(struct na_rastr *rastr);
static void flood_rastr2(struct na_rastr *rastr);
static void flood_fill(struct na_rastr *rastr, int i, int j, int val);
static struct na_rastr *make_bound(struct na_rastr *rastr, int bound);
static struct na_rastr *resize_rastr(struct na_rastr *rastr, int resize);

static double
calc_x(struct na_point *p1, struct na_point *p2, double y0)
{
	double x1, x2, y1, y2;

	x1 = p1->x;
	x2 = p2->x;
	y1 = p1->y;
	y2 = p2->y;

	return (-(x1 * y2 - x2 * y1) - (x2 - x1) * y0) / (y1 - y2);
}

static double *
get_intervals(double y1, double y2, int *nints)
{
	int i;
	double left, right, val, *intervals;

	*nints = 0;
	left = ceil(y1);
	if (fabs(y1 - left) > DBL_EPSILON)
		(*nints)++;
	right = floor(y2);
	if (fabs(y2 - right) > DBL_EPSILON)
		(*nints)++;

	*nints += right - left + 1;

	if ((intervals = malloc(sizeof(double) * (*nints))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}

	i = 0;
	if (fabs(y1 - left) > DBL_EPSILON)
		intervals[i++] = y1;

	for (val = left; val <= right; val += 1.0, i++)
		intervals[i] = val;

	if (fabs(y2 - right) > DBL_EPSILON)
			intervals[i] = y2;

	return intervals;

fail_exit:
	return NULL;
}

static void
flood_fill(struct na_rastr *rastr, int i, int j, int val)
{
	if (rastr->rastr_matrix[i][j] > 0)
		return;

	rastr->rastr_matrix[i][j] = val;

	if (i - 1 > 0)
		flood_fill(rastr, i - 1, j, val);

	if (i + 1 < rastr->height)
		flood_fill(rastr, i + 1, j, val);

	if (j - 1 > 0)
		flood_fill(rastr, i, j - 1, val);

	if (j + 1 < rastr->width)
		flood_fill(rastr, i, j + 1, val);
}

static void
flood_rastr(struct na_rastr *rastr)
{
	int i, j;
	for (i = 0; i < rastr->height; i++) {
		if (rastr->rastr_matrix[i][0] == 0)
			flood_fill(rastr, i, 0, 2);
		if (rastr->rastr_matrix[i][rastr->width - 1] == 0)
			flood_fill(rastr, i, rastr->width - 1, 2);
	}

	for (j = 0; j < rastr->width; j++) {
		if (rastr->rastr_matrix[0][j] == 0)
			flood_fill(rastr, 0, j, 2);
		if (rastr->rastr_matrix[rastr->height - 1][j] == 0)
			flood_fill(rastr, rastr->height - 1, j, 2);
	}

	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++)
			if (rastr->rastr_matrix[i][j] == 2)
				rastr->rastr_matrix[i][j] = 0;
			else if (rastr->rastr_matrix[i][j] != 3)
				rastr->rastr_matrix[i][j] = 1;
}


static void
flood_rastr2(struct na_rastr *rastr)
{
	int i, j, k;
	struct na_point *pts;

	for (k = 0, pts = rastr->outer_contour; k < rastr->npts; k++, pts++) {
		i = pts->y;
		j = pts->x;
		rastr->rastr_matrix[i][j] = 0;
	}

	for (i = 0; i < rastr->height; i++) {
		if (rastr->rastr_matrix[i][0] == 0)
			flood_fill(rastr, i, 0, 2);
		if (rastr->rastr_matrix[i][rastr->width - 1] == 0)
			flood_fill(rastr, i, rastr->width - 1, 2);
	}

	for (j = 0; j < rastr->width; j++) {
		if (rastr->rastr_matrix[0][j] == 0)
			flood_fill(rastr, 0, j, 2);
		if (rastr->rastr_matrix[rastr->height - 1][j] == 0)
			flood_fill(rastr, rastr->height - 1, j, 2);
	}

	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++)
			if (rastr->rastr_matrix[i][j] == 2)
				rastr->rastr_matrix[i][j] = 0;
			else if (rastr->rastr_matrix[i][j] == 0)
				rastr->rastr_matrix[i][j] = 2;

	for (k = 0, pts = rastr->outer_contour; k < rastr->npts; k++, pts++) {
		i = pts->y;
		j = pts->x;
		rastr->rastr_matrix[i][j] = 3;
	}

	for (i = 0; i < rastr->height; i++) {
		if (rastr->rastr_matrix[i][0] == 0)
			flood_fill(rastr, i, 0, 2);
		if (rastr->rastr_matrix[i][rastr->width - 1] == 0)
			flood_fill(rastr, i, rastr->width - 1, 2);
	}

	for (j = 0; j < rastr->width; j++) {
		if (rastr->rastr_matrix[0][j] == 0)
			flood_fill(rastr, 0, j, 2);
		if (rastr->rastr_matrix[rastr->height - 1][j] == 0)
			flood_fill(rastr, rastr->height - 1, j, 2);
	}

	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++)
			if (rastr->rastr_matrix[i][j] == 2)
				rastr->rastr_matrix[i][j] = 0;
			else if (rastr->rastr_matrix[i][j] != 3)
				rastr->rastr_matrix[i][j] = 1;
}

static void
mark_contour(struct na_rastr *rastr, int i, int j)
{
	int width, height;

	rastr->rastr_matrix[i][j] = 3;

	width = rastr->width;
	height = rastr->height;

	if (i - 1 >= 0 && rastr->rastr_matrix[i - 1][j] == 1)
			mark_contour(rastr, i - 1, j);

	if (i + 1 < height && rastr->rastr_matrix[i + 1][j] == 1)
		mark_contour(rastr, i + 1, j);

	if (j - 1 >= 0 && rastr->rastr_matrix[i][j - 1] == 1)
		mark_contour(rastr, i, j - 1);

	if (j + 1 < width && rastr->rastr_matrix[i][j + 1] == 1)
		mark_contour(rastr, i, j + 1);

	if (i - 1 >= 0 && j - 1 >= 0 && rastr->rastr_matrix[i - 1][j - 1] == 1)
		mark_contour(rastr, i - 1, j - 1);

	if (i - 1 >= 0 && j + 1 < width && rastr->rastr_matrix[i - 1][j + 1] == 1)
		mark_contour(rastr, i - 1, j + 1);

	if (i + 1 < height && j - 1 >= 0 && rastr->rastr_matrix[i + 1][j - 1] == 1)
		mark_contour(rastr, i + 1, j - 1);

	if (i + 1 < height && j + 1 < width && rastr->rastr_matrix[i + 1][j - 1] == 1)
		mark_contour(rastr, i + 1, j - 1);
}

static void
find_contour(struct na_rastr *rastr)
{
	int i;

	for (i = 0; i < rastr->height; i++)
		if (rastr->rastr_matrix[i][0] == 1)
			mark_contour(rastr, i, 0);
}

static struct na_rastr *
resize_rastr(struct na_rastr *rastr, int resize)
{
	int i, j, **rastr_matrix, **rastr_matrix2;
	struct na_rastr *rastr2;

	if ((rastr2 = malloc(sizeof(struct na_rastr))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}

	rastr2->width = rastr->width / resize + ((rastr->width % resize > 0) ? 1 : 0);
	rastr2->height = rastr->height / resize + ((rastr->height % resize > 0) ? 1 : 0);

	if ((rastr2->outer_contour = malloc(sizeof(struct na_point) *
										rastr2->width * rastr2->height)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_rastr;
	}

	if ((rastr2->rastr_matrix = malloc(sizeof(int *) * rastr2->height)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_outer_contour;
	}


	rastr_matrix2 = rastr2->rastr_matrix;
	for (i = 0; i < rastr2->height; i++, rastr_matrix2++)
		if ((*rastr_matrix2 = calloc(rastr2->width, sizeof(int))) == NULL) {
			i--;
			rastr_matrix2--;
			na_errno = NA_ENOMEM;
			goto fail_free_rows;
		}

	rastr_matrix2 = rastr2->rastr_matrix;
	rastr_matrix = rastr->rastr_matrix;
	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++)
			rastr_matrix2[i / resize][j / resize] = (rastr_matrix[i][j] > 0) ? 1 : 0;

	return rastr2;

fail_free_rows:
	for (; i >= 0; i--, rastr_matrix2--)
		free(*rastr_matrix2);
	free(++rastr_matrix2);

fail_free_outer_contour:
	free(rastr2->outer_contour);

fail_free_rastr:
	free(rastr2);

fail_exit:
	return NULL;
}

static struct na_rastr *
make_bound(struct na_rastr *rastr, int bound)
{
	int i, j, c, **rastr_matrix, **rastr_matrix2;
	struct na_rastr *rastr2;

	if ((rastr2 = malloc(sizeof(struct na_rastr))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}

	rastr2->width = rastr->width + bound * 2;
	rastr2->height = rastr->height + bound * 2;

	if ((rastr2->outer_contour = malloc(sizeof(struct na_point) *
										rastr2->width * rastr2->height)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_rastr;
	}

	if ((rastr2->rastr_matrix = malloc(sizeof(int *) * rastr2->height)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_free_outer_contour;
	}


	rastr_matrix2 = rastr2->rastr_matrix;
	for (i = 0; i < rastr2->height; i++, rastr_matrix2++)
		if ((*rastr_matrix2 = calloc(rastr2->width, sizeof(int))) == NULL) {
			i--;
			rastr_matrix2--;
			na_errno = NA_ENOMEM;
			goto fail_free_rows;
		}

	rastr_matrix2 = rastr2->rastr_matrix;
	rastr_matrix = rastr->rastr_matrix;
	for (i = 0; i < rastr->height; i++)
		for (j = 0; j < rastr->width; j++)
			rastr_matrix2[i + bound][j + bound] = rastr_matrix[i][j];

	for (c = 0; c < bound; c++) {
		for (i = 0; i < rastr2->height; i++)
			for (j = 0; j < rastr2->width; j++)
				if (rastr_matrix2[i][j] == 1) {
					rastr_matrix2[i + 1][j] = 2;
					rastr_matrix2[i - 1][j] = 2;
					rastr_matrix2[i][j + 1] = 2;
					rastr_matrix2[i][j - 1] = 2;
				}
		for (i = 0; i < rastr2->height; i++)
			for (j = 0; j < rastr2->width; j++)
				if (rastr_matrix2[i][j] == 2)
					rastr_matrix2[i][j] = 1;
	}

	return rastr2;

fail_free_rows:
	for (; i >= 0; i--, rastr_matrix2--)
		free(*rastr_matrix2);
	free(++rastr_matrix2);

fail_free_outer_contour:
	free(rastr2->outer_contour);

fail_free_rastr:
	free(rastr2);

fail_exit:
	return NULL;
}

struct na_rastr *
na_fig_to_rastr(struct na_figure *fig, enum NA_RASTR_TYPE rastr_type, int resize, int bound)
{
	int i, j, npts, **rastr_matrix;
		struct na_rastr *rastr;
		struct na_primitive *prims;
		struct na_point *pts;

		if (na_check_fig(fig) < 0) {
			na_errno = NA_EINVAL;
			goto fail_exit;
		}

		if ((rastr = malloc(sizeof(struct na_rastr))) == NULL) {
			na_errno = NA_ENOMEM;
			goto fail_exit;
		}

		rastr->width = (int)fig->width + 1;
		rastr->height = (int)fig->height + 1;

		if ((rastr->outer_contour = malloc(sizeof(struct na_point) *
											rastr->width * rastr->height)) == NULL) {
			na_errno = NA_ENOMEM;
			goto fail_free_rastr;
		}
		if ((rastr->rastr_matrix = malloc(sizeof(int *) * rastr->height)) == NULL)
			goto fail_free_outer_contour;

		rastr_matrix = rastr->rastr_matrix;
		for (i = 0; i < rastr->height; i++, rastr_matrix++)
			if ((*rastr_matrix = calloc(rastr->width, sizeof(int))) == NULL) {
				i--;
				rastr_matrix--;
				na_errno = NA_ENOMEM;
				goto fail_free_rows;
			}

		rastr_matrix = rastr->rastr_matrix;

		prims = fig->prims;

		for (i = 0; i < fig->nprims; i++, prims++) {
			for (j = 0, pts = prims->pts; j < prims->npts - 1; j++, pts++) {
				int k, nints;
				double y, x, x1, x2, step, *intervals;
				struct na_point top, bottom;

				if (pts->y > (pts + 1)->y) {
					top = *pts;
					bottom = *(pts + 1);
				} else {
					top = *(pts + 1);
					bottom = *pts;
				}


				if ((intervals = get_intervals(bottom.y, top.y, &nints)) == NULL) {
					i = rastr->height - 1;
					goto fail_free_rows;
				}
				if ((int)top.y - (int)bottom.y > 1) {
					for (k = 0; k < nints - 1; k++) {
						x1 = calc_x(&top, &bottom, intervals[k]);
						x2 = calc_x(&top, &bottom, intervals[k + 1]);
						y = intervals[k];

						step = (x2 > x1) ? 1.0 : -1.0;

						rastr_matrix[(int)y][(int)x1] = 1;
						rastr_matrix[(int)y][(int)x2] = 1;
						for (x  = trunc(x1); x != trunc(x2) + step; x += step)
							rastr_matrix[(int)y][(int)x] = 1;


					}
				} else {
					x1 = bottom.x;
					x2 = top.x;
					y = bottom.y;

					step = (x2 > x1) ? 1.0 : -1.0;

					rastr_matrix[(int)y][(int)x1] = 1;
					rastr_matrix[(int)y][(int)x2] = 1;
					for (x  = trunc(x1); x != trunc(x2) + step; x += step)
						rastr_matrix[(int)y][(int)x] = 1;
				}
			}
		}

		if (bound > 0) {
			struct na_rastr *tmp;

			tmp = make_bound(rastr, bound);
			na_destroy_rastr(rastr);
			free(rastr);

			if (tmp == NULL) {
				na_errno = NA_ENOMEM;
				goto fail_exit;
			}

			rastr = tmp;
		}

		if (resize > 1) {
			struct na_rastr *tmp;

			tmp = resize_rastr(rastr, resize);
			na_destroy_rastr(rastr);
			free(rastr);

			if (tmp == NULL) {
				na_errno = NA_ENOMEM;
				goto fail_exit;
			}

			rastr = tmp;
		}

		find_contour(rastr);

		npts = 0;
		for (i = 0; i <  rastr->height; i++)
			for (j = 0; j < rastr->width; j++)
				if (rastr->rastr_matrix[i][j] == 3)
					rastr->outer_contour[npts++] = na_point_new(j, i);

		rastr->npts = npts;
		if (rastr_type == NA_RASTR_TYPE_PART_IN_PART)
			flood_rastr2(rastr);
		else
			flood_rastr(rastr);

		return rastr;

	fail_free_rows:
		for (; i >= 0; i--, rastr_matrix--)
			free(*rastr_matrix);
		free(++rastr_matrix);

fail_free_outer_contour:
		free(rastr->outer_contour);

fail_free_rastr:
	free(rastr);

fail_exit:
	return NULL;
}

void
na_destroy_rastr(struct na_rastr *rastr)
{
	int i;

	for (i = 0; i < rastr->height; i++)
		free(rastr->rastr_matrix[i]);

	free(rastr->rastr_matrix);
}
