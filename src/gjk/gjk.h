#ifndef GJK_H
#define GJK_H

#include <stdbool.h>
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

struct simplex_t {
	struct vector_t points[3];

	int num_points;
};

/**
 * Add point to simplex
 * 
 * @return 0 if successful, -1 if it fails
 */
int simplex_add(struct vector_t v, struct simplex_t * s);

/**
 * Remove point from simplex
 *
 * @return 0 if successful, -1 if it fails
 */
int simplex_remove(int idx, struct simplex_t * s);

/**
 * For polygons, A and B, the minkowski difference is as follows
 * \f$ A - B = \{\pmb{a} - \pmb{b} | \pmb{a} \in A, \; \pmb{b} \in B\} \f$
 *
 * @param diff polygon_t where minkowski difference result will be stored. This structure must be pre-initialized with an allocated points array and num_points = poly1.num_points * poly2.num_points
 */
struct polygon_t minkowski_diff(struct polygon_t poly1, struct polygon_t poly2, struct polygon_t diff);

/**
 * \f$ \pmb{v} = s_B(\pmb{d}) = \text{arg max}_{v \in P} \pmb{v} \dot \pmb{d} \f$
 * 
 * @return Finds the point, v, on a polygon, P, furthest in the direction, d
 */
struct vector_t support(struct vector_t d, struct polygon_t poly);

/**
 * Checks whether poly1 and poly2 are intersecting (collision)
 *
 * @return true if there is a collision, false if no collision
 */
bool gjk_collision(struct polygon_t poly1, struct polygon_t poly2);

#ifdef __cplusplus
}
#endif

#endif
