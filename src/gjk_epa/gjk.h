#ifndef GJK_H
#define GJK_H

#include <stdbool.h>
#include "vector.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// I just chose numbers that looked good
#define MAX_ITERATIONS 1000

// Defines how fine the resolution of the simplex becomes for EPA when finding 
// the penetration vector. This matters for shapes with curves and an infinite number
// of points.
#define MAX_SIMPLEX_SIZE 256

struct simplex_t {
	struct vector_t points[MAX_SIMPLEX_SIZE];
	int num_points;
};

/**
 * Add point to simplex for use in GJK. Only allows for 1 to 3 points in the simplex.
 */
enum simplex_error_t simplex_add(struct vector_t v, struct simplex_t* s);

/**
 * Insert point, v, at idx. Allows adding up to MAX_SIMPLEX_SIZE points.
 */
enum simplex_error_t simplex_insert(struct vector_t v, int idx, struct simplex_t* s);

/**
 * Remove point from simplex
 */
enum simplex_error_t simplex_remove(int idx, struct simplex_t* s);

/**
 * For polygons, A and B, the minkowski difference is as follows
 * \f$ A - B = \{\pmb{a} - \pmb{b} | \pmb{a} \in A, \; \pmb{b} \in B\} \f$
 *
 * @param diff polygon_t where minkowski difference result will be stored. This structure must be pre-initialized with an allocated points array and num_points = poly1.num_points * poly2.num_points
 */
struct polygon_t minkowski_diff(struct polygon_t poly1, struct polygon_t poly2, struct polygon_t diff);

/**
 * \f$ \pmb{v} = s_B(\pmb{d}) = \text{arg max}_{v \in P} \pmb{v} \dot \pmb{d} \f$ where $P$ is the minkowski difference
 * 
 * @return Finds the point, v, furthest in the direction, d, on the minkowski difference of poly1 and poly 2,
 */
struct vector_t support(struct vector_t d, struct polygon_t poly1, struct polygon_t poly2);

/**
 * Checks whether poly1 and poly2 are intersecting (collision).
 * The `simplex` argument allows you to specify a struct simplex_t to store the simplex information.
 * If simplex is NULL, the function will create a simplex automatically.
 *
 * @return true if there is a collision, false if no collision
 */
bool gjk_collision(struct polygon_t poly1, struct polygon_t poly2, struct simplex_t* simplex);

#ifdef __cplusplus
}
#endif

#endif
