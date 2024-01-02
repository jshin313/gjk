/**
 * GJK Collision Detection for 2D with distance and closest point calculations
 * Currently only supports only polygons with finitely many points 
 *
 * Based on these resources:
 * https://youtu.be/ajv46BSqcK4?si=FwvnEZ4qwEg9061W&t=1756
 * https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 * https://dyn4j.org/2010/04/gjk-distance-closest-points/
 */

#include <string.h>
#include <stdlib.h>
#include "gjk.h"

const int MAX_ITERATIONS = 1000; // I just chose a number that looked good

const struct vector_t ORIGIN = {
	.x = 0,
	.y = 0,
	.z = 0,
};

int simplex_add(struct vector_t v, struct simplex_t* s) {
	if (s->num_points >= 3) {
		return -1;
	}

	s->points[s->num_points] = v;
	s->num_points++;

	return 0;
}

int simplex_remove(int idx, struct simplex_t* s) {
	if (s->num_points <= 0) {
		return -1;
	}

	// Shift all elements down one
	//
	// e.g. simplex_remove(0, s) if we have points = {4, 5, 6}
	// should yield points = {5, 6}
	if (idx < s->num_points - 1) {
		for (int i = 0; i < s->num_points - idx - 1; i++) {
			s->points[idx+i] = s->points[idx+1+i];
		}
	}
	s->num_points--;

	return 0;
}

/**
 * @param diff Pointer to polygon_t where minkowski difference result will be stored. This structure must be pre-initialized with an allocated points array and num_points = poly1.num_points * poly2.num_points
 *
 */
struct polygon_t minkowski_diff(struct polygon_t poly1, struct polygon_t poly2, struct polygon_t diff) {

	for (int i = 0; i < poly1.num_points; i++) {
		for (int j = 0; j < poly2.num_points; j++) {
			diff.points[i*(poly2.num_points) + j] = sub(poly1.points[i], poly2.points[j]);
		}
	}

	return diff;
}

struct vector_t support(struct vector_t d, struct polygon_t poly) {
	int max_dp = dot(poly.points[0], d);
	struct vector_t v = poly.points[0];

	for (int i = 1; i < poly.num_points; i++) {
		int dp = dot(poly.points[i], d);

		if (dp > max_dp) {
			max_dp = dp;
			v = poly.points[i];
		}
	}

	return v;
}

// Note this is for handling a triangular simplex, GJK works for arbitrary polygon
bool triangle_case(struct simplex_t* s, struct vector_t* d) {
	struct vector_t c =  s->points[0];
	struct vector_t b = s->points[1];
	struct vector_t a = s->points[2];

	struct vector_t ab = sub(b, a);
	struct vector_t ac = sub(c, a);
	struct vector_t ao = sub(ORIGIN, a);

	struct vector_t ab_perp = triple_product2(ac, ab, ab);
	struct vector_t ac_perp = triple_product2(ab, ac, ac);

	if (dot(ab_perp, ao) > 0) { // Region AB
		simplex_remove(0, s);
		*d = ab_perp;
		return false;
	} else if (dot(ac_perp, ao) > 0) { // Region AC
		simplex_remove(1, s);
		*d = ac_perp;
		return false;
	}
	return true;
}

bool line_case(struct simplex_t* s, struct vector_t* d) {
	struct vector_t b = s->points[0];
	struct vector_t a = s->points[1];

	struct vector_t ab = sub(b, a);
	struct vector_t ao = sub(ORIGIN, a);

	struct vector_t ab_perp = triple_product2(ab, ao, ab);

	*d = ab_perp;
	return false;
}

bool contains_origin(struct simplex_t* s, struct vector_t* d) {
	if (s->num_points == 2) {
		return line_case(s, d);
	}
	return triangle_case(s, d);
}

bool gjk_collision(struct polygon_t poly1, struct polygon_t poly2) { 
	// direction d to check = poly2.center - poly1.center
	struct vector_t d = sub(get_centroid(poly2), get_centroid(poly1));

	struct simplex_t simplex =  {
		.num_points = 0
	};
	
	// Stores the minkowski difference
	struct polygon_t diff = {
		.num_points = poly1.num_points * poly2.num_points,
		.points = (struct vector_t* ) alloca(poly1.num_points * poly2.num_points * sizeof (struct vector_t)),
	};
	
	simplex_add(support(d, minkowski_diff(poly1, poly2, diff)), &simplex);
	
	d = sub(ORIGIN, simplex.points[0]);
	/* d = sub(ORIGIN, d); */

	for (int iterations = 0; iterations < MAX_ITERATIONS; iterations++) {
		struct vector_t A = support(d, minkowski_diff(poly1, poly2, diff));

		if (dot(A, d) < 0) {
			return false;
		}

		simplex_add(A, &simplex);

		if (contains_origin(&simplex, &d)) {
			return true;
		}
	}

	// If GJK doesn't converge after MAX_ITERATIONS, assume the polygons don't intersect
	return false;
}
