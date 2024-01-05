#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include "fixed_point.h"

fixed_point_Q16_t int_to_fixed_point(int64_t i) {
	return i * (1 << 16);
}

void polygon_t_int_to_fixed_point(struct polygon_t P) {
	for (int i = 0; i < P.num_points; i++) {
		P.points[i].x = int_to_fixed_point(P.points[i].x);
		P.points[i].y = int_to_fixed_point(P.points[i].y);
	}
}

/** Truncates the fractional part
 * @return floor(f)
 */
int64_t fixed_point_to_int(fixed_point_Q16_t f) {
	return f/(1 << 16);
}

int64_t get_remainder(fixed_point_Q16_t f) {
	return f % (1 << 16);
}

#endif
