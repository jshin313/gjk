/**
 * Implements common euclidean vector operations
 *
 * I treat points as euclidean vectors since they're basically the same
 */

#include "vector.h"

int64_t dot(struct vector_t v1, struct vector_t v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

struct vector_t sub(struct vector_t v1, struct vector_t v2) {
	return (struct vector_t) {
		.x = v1.x-v2.x,
		.y = v1.y-v2.y,
	};
}

struct vector_t scalar_mult(int32_t s, struct vector_t v) {
	return (struct vector_t) {
		.x= s * v.x,
		.y= s * v.y,
	};
}

struct vector_t triple_product2(struct vector_t v1, struct vector_t v2, struct vector_t v3) {
	return sub(scalar_mult(dot(v1, v3), v2), scalar_mult(dot(v3, v2), v1));
}

/**
 * https://en.wikipedia.org/wiki/Integer_square_root#Example_implementation_in_C
 */
unsigned int int_sqrt(unsigned int s) {
	// Zero yields zero
    // One yields one
	if (s <= 1) 
		return s;

    // Initial estimate (must be too high)
	unsigned int x0 = s / 2;

	// Update
	unsigned int x1 = (x0 + s / x0) / 2;

	while (x1 < x0)	// Bound check
	{
		x0 = x1;
		x1 = (x0 + s / x0) / 2;
	}		
	return x0;
}

/*
 * Won't be exact since int is used instead of floats, but we don't need too much precision, since we're only checking 
 * if vectors are < 0
 *
 * Use norm so that integers don't get really big.
 *
 */
struct vector_t normalize_if_norm_nonzero(struct vector_t v) {
	int dp = dot(v, v);

	if (dp == 0) {
		return v;
	}

	return (struct vector_t) {
		.x= v.x/int_sqrt(dot(v, v)),
		.y= v.y/int_sqrt(dot(v, v)),
	};
}

/*
 * Won't be exact, but we don't need precision (see above)
 */
struct vector_t get_centroid(struct polygon_t poly) {
	int sum_x = 0, sum_y = 0;

	for (int i = 0; i < poly.num_points; i++) {
		sum_x += poly.points[i].x;
		sum_y += poly.points[i].y;
	}

	return (struct vector_t) {
		.x= sum_x/poly.num_points,
		.y= sum_y/poly.num_points,
	};
}
