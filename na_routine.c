#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "na_errno.h"
#include "na_routine.h"

#define MAX_FIGS_DEFAULT 	1024
#define MAX_PRIMS_DEFAULT 	1024
#define MAX_PTS_DEFAULT 	1024
#define STR_LENGTH 			2048

#define FIG_SEPARATOR 		":\n"
#define PRIM_SEPARATOR		"\n"

enum STATE {
	STATE_NEWFIG,
	STATE_PRIM
};

static void fig_init(struct na_figure *fig);
static void move_to_zero(struct na_figure *fig);

int 
na_check_fig(struct na_figure *fig) {
	int i;
	struct na_primitive *prims;
	
	prims = fig->prims;
	
	if (fig == NULL)
		goto fail_exit;

	if (fig->quant <= 0 || fig->nprims <= 0 || fig->angstep < 0 || fig->prims == NULL)
		goto fail_exit;

	for (i = 0; i < fig->nprims; i++, prims++)
		if (prims->npts <= 0 || prims->pts == NULL)
			goto fail_exit;

	return 0;

fail_exit:
	return -1;
}

void
na_destroy_fig(struct na_figure *fig)
{
	int i;
	struct na_primitive *prims;
	
	prims = fig->prims;
	for (i = 0; i < fig->nprims; i++, prims++)
		free(prims->pts);
	free(fig->prims);
}

int
na_copy_fig(struct na_figure *dest, struct na_figure *src)
{
	int i;
	struct na_primitive *src_prims, *dest_prims;
	if (na_check_fig(src) < 0) {
		na_errno = NA_EINVAL;
		goto fail_exit;
	}

	dest->id = src->id;
	dest->quant = src->quant;
	dest->nprims = src->nprims;
	dest->angstep = src->angstep;
	dest->mass_center = src->mass_center;
	
	for (i = 0; i < 3; i++)
		memcpy(dest->transform[i], src->transform[i], sizeof(double) * 3);
	

	if ((dest->prims = malloc(sizeof(struct na_primitive) * src->nprims)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}

	src_prims = src->prims;
	dest_prims = dest->prims;
	for (i = 0; i < src->nprims; i++, src_prims++, dest_prims++) {
		dest_prims->npts = src_prims->npts;
		if ((dest_prims->pts = malloc(sizeof(struct na_point) * src_prims->npts)) == NULL) {
			i--;
			dest_prims--;
			na_errno = NA_ENOMEM;
			goto fail_pts;
		}

		memcpy(dest_prims->pts, src_prims->pts, sizeof(struct na_point) * src_prims->npts);
	}

	return 0;

fail_pts:
	free(dest->prims);
	for (; i >= 0; i--, dest_prims--)
		free(dest_prims->pts);

fail_exit:
	return -1;
}

struct na_figure *
na_makeset(struct na_figure *figs, int nfigs, int *setsize)
{
	int i, j, k;
	struct na_figure *figs_ptr, *figset, *figset_ptr;

	(*setsize) = 0;

	figs_ptr = figs;
	for (i = 0; i < nfigs; i++, figs++) {
		if (na_check_fig(figs) < 0) {
			na_errno = NA_EINVAL;
			goto fail_exit;
		}

		(*setsize) += figs->quant;
	}
	
	if ((figset = malloc(sizeof(struct na_figure) * (*setsize))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	}
	
	figset_ptr = figset;
	figs = figs_ptr;
	for (i = 0, j = 0; i < nfigs; i++, figs++)
		for (k = 0; k < figs->quant; k++, j++, figset++)
			if (na_copy_fig(figset, figs) < 0) {
				j--;
				figset--;
				goto fail_copy;
			}

	return figset_ptr;

fail_copy:
	for (; j >= 0; j--, figset--)
		na_destroy_fig(figset);
	free(figset_ptr);
		
fail_exit:
	return NULL;
}

int 
na_calc_mass_center(struct na_figure *fig)
{
	int i, j;
	double xsum, ysum;
	struct na_primitive *prims;

	if (na_check_fig(fig) < 0) {
		na_errno = NA_ENOMEM;
		goto exit_fail;
	}

	prims = fig->prims;
	xsum = ysum = 0.0;
	for (i = 0; i < fig->nprims; i++, prims++) {	
		struct na_point *pts;
		
		pts = prims->pts;
		for (j = 0; j < prims->npts; j++, pts++) {
			xsum += pts->x;
			ysum += pts->y;
		}
	}

	fig->mass_center.x = xsum / fig->nprims;
	fig->mass_center.x = ysum / fig->nprims;

exit_fail:
	return -1;
}

static void
move_to_zero(struct na_figure *fig)
{
}

static void
fig_init(struct na_figure *fig)
{
	int i, j;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			fig->transform[i][j] = (i == j) ? 1 : 0;

	na_calc_gc(fig);
}

struct na_figure *
na_read_figs(FILE *file, int *nfigs)
{
	int i;
	int max_figs, max_prims, max_pts;
	int nprims, npts;
	char str[STR_LENGTH];
	enum STATE state = STATE_NEWFIG;
	struct na_figure *figs;
	struct na_primitive *prims;
	struct na_point *pts;

	if (file == NULL || nfigs == NULL) {
		na_errno = NA_EINVAL;
		goto exit_fail;
	}

	(*nfigs) = nprims = npts = 0;
	max_figs = MAX_FIGS_DEFAULT;
	max_prims = MAX_PRIMS_DEFAULT;
	max_pts = MAX_PTS_DEFAULT;
	
	if ((figs = malloc(sizeof(struct na_figure) * max_figs)) == NULL) {
		na_errno = NA_ENOMEM;
		goto exit_fail;
	}
	
	while (fgets(str, STR_LENGTH, file)) {
		double x, y;

		if (state == STATE_NEWFIG) {
			figs[*nfigs].id = *nfigs;

			if (sscanf(str, "%d %d\n", &(figs[*nfigs].quant), &(figs[*nfigs].angstep)) != 2) {
				na_errno = NA_EFIGFORMAT;
				goto fail_newfig;
			}

			max_prims = MAX_PRIMS_DEFAULT;
			max_pts = MAX_PTS_DEFAULT;
			state = STATE_PRIM;

			if ((prims = malloc(sizeof(struct na_primitive) * max_prims)) == NULL) {
				na_errno = NA_ENOMEM;
				goto fail_newfig;
			}

			if ((pts = malloc(sizeof(struct na_point) * max_pts)) == NULL) {
				free(prims);
				na_errno = NA_ENOMEM;
				goto fail_newfig;
			}
			
			nprims = npts = 0;
			
			continue;
		}

		if (strcmp(str, FIG_SEPARATOR) == 0) {
			state = STATE_NEWFIG;
			prims[nprims].npts = npts;
			prims[nprims].pts = pts;
			figs[*nfigs].nprims = ++nprims;
			figs[(*nfigs)++].prims = prims;


			if (*nfigs == max_figs) {
				void *tmp;
				
				max_figs *= 2;
				if ((tmp = realloc(figs, sizeof(struct na_figure) * max_figs)) == NULL) {
					na_errno = NA_ENOMEM;
					goto fail_complete_fig;
				}
				figs = tmp;
			}
			
			continue;
		}

		if (strcmp(str, PRIM_SEPARATOR) == 0) {
			prims[nprims].npts = npts;
			prims[nprims++].pts = pts;
			if (nprims == max_prims) {
				void *tmp;

				max_prims *= 2;
				if ((tmp = realloc(prims, sizeof(struct na_primitive) * max_prims)) == NULL) {
					na_errno = NA_ENOMEM;
					goto fail_free_pts;
				}

				prims = tmp;
			}

			max_pts = MAX_PTS_DEFAULT;
			if ((pts = malloc(sizeof(struct na_point) * max_pts)) == NULL) {
				free(prims);
				na_errno = NA_ENOMEM;
				goto fail_free_prims;
			}
			
			npts = 0;

			continue;
		}

		if (sscanf(str, "%lf %lf\n", &x, &y) != 2) {
			na_errno = NA_EFIGFORMAT;
			goto fail_free_pts;
		}

		pts[npts].x = x;
		pts[npts++].y = y;

		if (npts == max_pts) {
			void *tmp;

			max_pts *= 2;
			if ((tmp = realloc(pts, sizeof(struct na_point) * max_pts)) == NULL) {
				na_errno = NA_ENOMEM;
				goto fail_free_pts;
			}
		}
	}

	for (i = 0; i < nfigs; i++)
		fig_init(&figs[i]);

	return figs;

fail_free_pts:
	free(pts);
fail_free_prims:
	free(prims);
fail_newfig:
	(*nfigs)--;
fail_complete_fig:
	for (; (*nfigs) >= 0; (*nfigs)--)
		na_destroy_fig(figs + (*nfigs));
	free(figs);

exit_fail:
	return NULL;
}
