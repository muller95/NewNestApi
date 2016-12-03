#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "na_genetics.h"
#include "na_rastr.h"
#include "na_rastr_nest.h"
#include "na_routine.h"
#include "na_transform.h"

void
rastr_to_file(struct na_rastr *rastr, char *path)
{
	int i, j;
	FILE *file;


	if ((file = fopen(path, "w+")) == NULL) {
		printf("error opening file\n");
		exit(1);
	}

	for (i = 0; i < rastr->height; i++) {
		for (j = 0; j < rastr->width; j++)
			fprintf(file, "%d", rastr->rastr_matrix[i][j]);
		fprintf(file, "\n");
	}
	fflush(file);
	fclose(file);

}


void
figs_to_file(struct na_figure *figs, int nfigs, char *path)
{
	int i, j, k;
	char name[255];
	FILE *file;
	struct na_primitive *prims;
	struct na_point *pts;

	for (i = 0; i < nfigs; i++, figs++) {
		prims = figs->prims;
		bzero(name, 255);
		sprintf(name, "%s%d", path, i);
		if ((file = fopen(name, "w+")) == NULL) {
			printf("error opening file\n");
			exit(1);
		}
		for (j = 0; j < figs->nprims; j++, prims++) {
			pts = prims->pts;
			for (k = 0; k < prims->npts - 1; k++, pts++) {
				fprintf(file, "%lf\n", pts->x);
				fprintf(file, "%lf\n", pts->y);
				fprintf(file, "%lf\n", (pts + 1)->x);
				fprintf(file, "%lf\n", (pts + 1)->y);
			}
		}
		fflush(file);
		fclose(file);
	}
}

int
main()
{
	int nfigs, set_size, nested, need;
	int max_indivs;
	int i, j, k, m;
	FILE *f;
	struct na_figure *figs, *fig_set;
	struct na_individ indivs;
	struct na_nest_attrs attrs;

	printf("here\n");
	if ((f = fopen("/home/vadim/workspace/NestAPI/Debug/data", "r")) == NULL) {
		printf("error opening file\n");
		return -1;
	}


	if ((figs = na_read_figs(f, &nfigs)) == NULL) {
		printf("error reading figs\n");
		return -1;
	}

	if ((fig_set = na_makeset(figs, nfigs, &set_size)) == NULL) {
		printf("error on makiing set\n");
		return 1;
	}

	indivs.genom_size = 0;
	attrs.rastr_type = NA_RASTR_TYPE_PART_IN_PART;
	attrs.bound = 2;
	attrs.resize = 0;

	nested = 0;
	need = set_size;

		na_rastr_nest(fig_set, set_size, &indivs, 1000, 1000, &attrs);

		for (i = 0; i < indivs.genom_size; i++) {
		double a, b, c, d, e, f;

		a = indivs.posits[i].fig->matrix[0][0];
		b = indivs.posits[i].fig->matrix[1][0];
		c = indivs.posits[i].fig->matrix[0][1];
		d = indivs.posits[i].fig->matrix[1][1];
		e = indivs.posits[i].fig->matrix[0][2];
		f = indivs.posits[i].fig->matrix[1][2];

		printf("matrix(%lf, %lf, %lf, %lf, %lf, %lf)\n:\n", a, b, c, d, e, f);
	}

	printf("-\n");

	printf("done\n");
	return 0;
}
