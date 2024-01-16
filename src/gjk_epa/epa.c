#include "epa.h"
#include "gjk.h"
#include "fixed_point.h"
#include <limits.h>

enum {
	CLOCKWISE,
	COUNTERCLOCKWISE
};

// Based on https://dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
// and https://blog.hamaluik.ca/posts/building-a-collision-engine-part-2-2d-penetration-vectors/
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

struct vector_t epa(struct polygon_t poly1, struct polygon_t poly2) {
	polygon_t_int_to_fixed_point(poly1);
	polygon_t_int_to_fixed_point(poly2);

	struct simplex_t simplex =  {
		.num_points = 0
	};

	if (!gjk_collision(poly1, poly2, &simplex)) {
		return (struct vector_t){0, 0};
	}

	int64_t e0 = (simplex.points[1].x - simplex.points[0].x) * (simplex.points[1].y + simplex.points[0].y);
	int64_t e1 = (simplex.points[2].x - simplex.points[1].x) * (simplex.points[2].y + simplex.points[1].y);
	int64_t e2 = (simplex.points[0].x - simplex.points[2].x) * (simplex.points[0].y + simplex.points[2].y);
	int winding = (e0 + e1 + e2 >= 0) ? CLOCKWISE: COUNTERCLOCKWISE;

	for (int i = 0; i < MAX_ITERATIONS; i++) {
		struct edge_t e = find_closest_edge(winding, &simplex);
		struct vector_t p = support(e.normal, poly1, poly2);

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
