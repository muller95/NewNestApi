#include "na_figure.h"

struct na_point
na_point_new(double x, double y)
{
	struct na_point point;

	point.x = x;
	point.y = y;

	return point;
}
