#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "na_routine.h"
#include "na_transform.h"
#include "na_trigon.h"

static void mult_matrixes(double matrix[3][3], struct na_figure *fig);

void
na_rotate(struct na_figure *fig, int angle)
{
	int i;
	double matrix[3][3];

	for (i = 0; i < 3; i++)
		bzero(matrix[i], sizeof(double) * 3);

	matrix[0][0] = na_cosine[angle];
	matrix[0][1] = na_sine[angle];
	matrix[1][0] = -na_sine[angle];
	matrix[1][1] = na_cosine[angle];
	matrix[2][2] = 1;

	na_transform(fig, matrix);
	na_move_to_zero(fig);
	na_calc_width_height(fig);
	na_calc_mass_center(fig);
}

void
na_move_to_zero(struct na_figure *fig)
{
	int i, j;
	struct na_primitive *prims;
	struct na_point *pts;
	double a, b;

	prims = fig->prims;
	a = prims->pts[0].x;
	b = prims->pts[0].y;

	for (i = 0; i < fig->nprims; i++, prims++)
		for (j = 0, pts = prims->pts; j < prims->npts; j++, pts++) {
			a = (a > pts->x) ? pts->x : a;
			b = (b > pts->y) ? pts->y : b;
		}

	na_translate(fig, -a, -b);
}

void
na_translate(struct na_figure *fig, double a, double b)
{
	int i, j;
	double matrix[3][3];

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			matrix[i][j] = (i == j) ? 1 : 0;

	matrix[0][2] = a;
	matrix[1][2] = b;

	na_transform(fig, matrix);
	fig->mass_center.x += a;
	fig->mass_center.y += b;
}

static void
mult_matrixes(double matrix[3][3], struct na_figure *fig)
{
	int i, j, r;
	double result[3][3];

	for (i = 0; i < 3; i++)
			bzero(result[i], sizeof(double) * 3);

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			for (r = 0; r < 3; r++)
				result[i][j] += matrix[i][r] * fig->matrix[r][j];

	for (i = 0; i < 3; i++)
		memcpy(fig->matrix[i], result[i], sizeof(double) * 3);
}

void
na_transform(struct na_figure *fig, double matrix[3][3])
{
	int i, j;
	double x ,y;
	struct na_primitive *prims;
	struct na_point *pts;

	prims = fig->prims;
	for (i = 0; i < fig->nprims; i++, prims++)
		for (j = 0, pts = prims->pts; j < prims->npts; j++, pts++) {
			x = matrix[0][0] * pts->x + matrix[0][1] * pts->y + matrix[0][2];
			y = matrix[1][0] * pts->x + matrix[1][1] * pts->y + matrix[1][2];
			*pts = na_point_new(x, y);
		}

	mult_matrixes(matrix, fig);
}
