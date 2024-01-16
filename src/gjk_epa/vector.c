/**
 * Implements common euclidean vector operations
 *
 * I treat points as euclidean vectors since they're basically the same
 */

#include "vector.h"
#include "fixed_point.h"

int64_t dot(struct vector_t v1, struct vector_t v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

struct vector_t sub(struct vector_t v1, struct vector_t v2) {
	return (struct vector_t) {
		.x = v1.x-v2.x,
		.y = v1.y-v2.y,
	};
}

struct vector_t scalar_mult(int64_t s, struct vector_t v) {
	return (struct vector_t) {
		.x= s * v.x,
		.y= s * v.y,
	};
}

// Note: Values sometimes overflow
struct vector_t triple_product2(struct vector_t v1, struct vector_t v2, struct vector_t v3) {
	int64_t v1_v3_dp = dot(v1, v3);
	int64_t v3_v2_dp = dot(v3, v2);
	struct vector_t t1 = scalar_mult(v1_v3_dp, v2);
	struct vector_t t2 = scalar_mult(v3_v2_dp, v1);
	return sub(t1, t2);
}

/**
 * https://en.wikipedia.org/wiki/Integer_square_root#Example_implementation_in_C
 */
int64_t int_sqrt(int64_t s) {
	// Zero yields zero
    // One yields one
	if (s <= 1) 
		return s;

    // Initial estimate (must be too high)
	int64_t x0 = s / 2;

	// Update
	int64_t x1 = (x0 + s / x0) / 2;

	while (x1 < x0)	// Bound check
	{
		x0 = x1;
		x1 = (x0 + s / x0) / 2;
	}		
	return x0;
}

/*
 * Won't be exact since int is used instead of floats
 *
 * Use norm so that integers don't get really big.
 *
 */
struct vector_t normalize(struct vector_t v) {
	int64_t dp = dot(v, v);

	if (dp == 0) {
		return v;
	}

	int64_t norm = int_sqrt(dp);
	
	// Anytime division is done, convert to fixed point again since division cancels out the scaling factor
	v.x = int_to_fixed_point(v.x);
	v.y = int_to_fixed_point(v.y);

	v.x = v.x/norm;
	v.y = v.y/norm;

	return v; 
}

/*
 * Won't be exact, but we don't need precision
 */
struct vector_t get_centroid(struct polygon_t poly) {
	int64_t sum_x = 0, sum_y = 0;

	for (int i = 0; i < poly.num_points; i++) {
		sum_x += poly.points[i].x;
		sum_y += poly.points[i].y;
	}

	return (struct vector_t) {
		.x= sum_x/poly.num_points,
		.y= sum_y/poly.num_points,
	};
}
