#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "na_errno.h"
#include "na_genetics.h"


int 
na_check_individ(struct na_individ *indiv)
{
	if (indiv->genom_size <= 0 || indiv->genom == NULL)
		return 1;
	
	return 0;
}


struct na_individ *
na_mutate(struct na_individ *indiv)
{
	int n1, n2, tmp, *genom;
	struct na_individ *mutant;

	if (na_check_individ(indiv) != 0 || indiv->genom_size <= 0) {
		na_errno = NA_EINVAL;
		goto fail_exit;
	}

	if ((mutant = malloc(sizeof(struct na_individ))) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_exit;
	} 

	mutant->genom_size = indiv->genom_size;
	if ((genom = malloc(sizeof(int) * indiv->genom_size)) == NULL) {
		na_errno = NA_ENOMEM;
		goto fail_alloc_genom;
	}

	memcpy(genom, indiv->genom, sizeof(int) * indiv->genom_size);
	
	srand(time(NULL));

	n1 = rand() % indiv->genom_size;
	n2 = rand() % indiv->genom_size;

	while (n1 == n2)
		n2 = rand() % indiv->genom_size;

	tmp = genom[n1];
	genom[n1] = indiv->genom[n2];
	indiv->genom[n2] = tmp;

	return mutant;
	
fail_alloc_genom:
	free(mutant);
fail_exit:
	return NULL;
}
