#ifndef _NA_FIGURE_H
#define _NA_FIGURE_H

struct na_point {
	double x, y;
};

struct na_primitive {
	int npts;
	struct na_point *pts;
};

struct na_figure {
	int id, quant, nprims, angstep;
	double matrix[3][3], width, height;
	struct na_primitive *prims;
	struct na_point mass_center;
};

struct na_point na_point_new(double x, double y);

#endif

