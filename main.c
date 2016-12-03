#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "na_routine.h"

int
main()
{
	int nfigs, set_size, i, j, k;
	FILE *f;
	struct na_figure *figs, *copy = NULL, *fig_set, tmp[10];

	printf("here\n");
	if ((f = fopen("/home/vadim/workspace/NestAPI/Debug/data", "r")) == NULL) {
		printf("error opening file\n");
		return -1;
	}


	if ((figs = na_read_figs(f, &nfigs)) == NULL) {
		printf("error reading figs\n");
		return -1;
	}

	copy = malloc(sizeof(struct na_figure));
	if (na_copy_fig(copy, &figs[0]) < 0) {
		printf("error on copying fig\n");
		return -1;
	}
	na_destroy_fig(copy);
	free(copy);

	/*for (i = 0, j = 0; i < nfigs; i++)
		for (k = 0; k < figs[i].quant; k++, j++)
			na_copy_fig(&tmp[j], &figs[i]);

	for (i = 0; i < 10; i++)
			na_destroy_fig(&tmp[i]);*/


	if ((fig_set = na_makeset(figs, nfigs, &set_size)) == NULL) {
		printf("error on makiing set\n");
		return 1;
	}

	for (i = 0; i < set_size; i++)
		na_destroy_fig(&fig_set[i]);


	int  a = 2 + 2;

	printf("done\n");
	return 0;
}
