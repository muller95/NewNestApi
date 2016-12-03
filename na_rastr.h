#ifndef _NA_RASTR_H
#define _NA_RASTR_H

struct na_rastr {
	int **rastr;
	int w, h;
};

struct na_rastr *na_fig_to_rastr(struct na_figure *fig);
void na_destr_rastr(struct na_rastr *rastr);

#endif

