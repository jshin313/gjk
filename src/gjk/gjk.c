/**
 * GJK Collision Detection for 2D with distance
 * Currently only supports only polygons with finitely many points 
 *
 * Based on these resources:
 * https://youtu.be/ajv46BSqcK4?si=FwvnEZ4qwEg9061W&t=1756
 * https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 */

#include <string.h>
#include <alloca.h>
#include <limits.h>
#include "gjk.h"
#include "fixed_point.h"

// I just chose numbers that looked good
const int MAX_ITERATIONS = 1000;
const int TOLERANCE = 1;

enum {
	CLOCKWISE,
	COUNTERCLOCKWISE
};

const struct vector_t ORIGIN = {
	.x = 0,
	.y = 0,
};


struct simplex_t simplex =  {
	.num_points = 0
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
	int64_t max_dp = dot(poly.points[0], d);
	struct vector_t v = poly.points[0];

	for (int i = 1; i < poly.num_points; i++) {
		int64_t dp = dot(poly.points[i], d);

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
	simplex.num_points = 0;	

	// direction d to check = poly2.center - poly1.center
	/* struct vector_t d = (struct vector_t) {.x=1, .y=0}; */
	struct vector_t d = (struct vector_t) {.x=int_to_fixed_point(1), .y=0};
	/* struct vector_t d = sub(get_centroid(poly2), get_centroid(poly1)); */

	// Stores the minkowski difference
	struct polygon_t diff = {
		.num_points = poly1.num_points * poly2.num_points,
		.points = (struct vector_t* ) alloca(poly1.num_points * poly2.num_points * sizeof (struct vector_t)),
	};

	simplex_add(support(d, minkowski_diff(poly1, poly2, diff)), &simplex);

	/* d = sub(ORIGIN, simplex.points[0]); */
	d = sub(ORIGIN, d);

	for (int iterations = 0; iterations < MAX_ITERATIONS; iterations++) {
		struct vector_t A = support(d, minkowski_diff(poly1, poly2, diff));

		simplex_add(A, &simplex);


		if (dot(A, d) < 0) {
			return false;
		}

		if (contains_origin(&simplex, &d)) {
			return true;
		}
	}

	// If GJK doesn't converge after MAX_ITERATIONS, assume the polygons don't intersect
	return false;
}

// Based on https://dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
// and https://blog.hamaluik.ca/posts/building-a-collision-engine-part-2-2d-penetration-vectors/
// might not match what you want for corners since EPA finds vector that separates polygons along the shortest path
struct edge_t find_closest_edge(int winding, struct simplex_t* s) {
	struct edge_t closest;

	closest.distance = INT_MAX;
	for (int i = 0; i < s->num_points; i++) {
		// compute the next points index
		int j = i + 1 == s->num_points ? 0 : i + 1;
		// get the current point and the next one
		struct vector_t a = s->points[i];
		struct vector_t b = s->points[j];
		// create the edge vector
		struct vector_t e = sub(b, a); // a to b;
									   // get the vector from the origin to a
									   // get the vector from the edge towards the origin
		struct vector_t n;
		if (winding == CLOCKWISE) {
			// (y, -x)
			n.x = -e.y;
			n.y = e.x;
		} else {
			// (-y, x)
			n.x = e.y;
			n.y = -e.x;
		}

		// normalize the vector
		n = normalize(n);
		int64_t d = fixed_point_to_int(dot(n, a));

		// check the distance against the other distances
		if (d!= 0 && d < closest.distance) {
			// if this edge is closer then use it
			closest.distance = d;
			closest.normal = n;
			closest.index = j;
		}

	}
	return closest;
}

int simplex_insert(struct vector_t v, int idx, struct simplex_t* s) {
	if (s->num_points >= MAX_SIMPLEX_SIZE) {
		return -1;
	}

	// Shift all elements to the right by one
	for (int i = s->num_points; i >= idx + 1; i--) {
		s->points[i] = s->points[i-1];
	}

	s->points[idx] = v;
	s->num_points++;

	return 0;
}

struct vector_t epa(struct polygon_t poly1, struct polygon_t poly2) {
	polygon_t_int_to_fixed_point(poly1);
	polygon_t_int_to_fixed_point(poly2);

	if (!gjk_collision(poly1, poly2)) {
		return (struct vector_t){0, 0};
	}

	// Stores the minkowski difference
	struct polygon_t diff = {
		.num_points = poly1.num_points * poly2.num_points,
		.points = (struct vector_t* ) alloca(poly1.num_points * poly2.num_points * sizeof (struct vector_t)),
	};

	int64_t e0 = (simplex.points[1].x - simplex.points[0].x) * (simplex.points[1].y + simplex.points[0].y);
	int64_t e1 = (simplex.points[2].x - simplex.points[1].x) * (simplex.points[2].y + simplex.points[1].y);
	int64_t e2 = (simplex.points[0].x - simplex.points[2].x) * (simplex.points[0].y + simplex.points[2].y);
	int winding = (e0 + e1 + e2 >= 0) ? CLOCKWISE: COUNTERCLOCKWISE;

	for (int i = 0; i < MAX_ITERATIONS; i++) {
		struct edge_t e = find_closest_edge(winding, &simplex);
		struct vector_t p = support(e.normal, minkowski_diff(poly1, poly2, diff));

		// dot product scaling_factor^2, so divide by scaling factor again to get back to fixed_point
		int64_t d = fixed_point_to_int(dot(p, e.normal));

		if (d - e.distance < TOLERANCE || simplex.num_points >= MAX_SIMPLEX_SIZE) {
			struct vector_t fp_result = scalar_mult(d, e.normal);
			return (struct vector_t) {
				.x=fixed_point_to_int(fp_result.x),
					.y=fixed_point_to_int(fp_result.y),
			};
		} else {
			simplex_insert(p, e.index, &simplex);
		}
	}

	return (struct vector_t){0, 0};
}
