/**
 * Implements common euclidean vector operations
 *
 * I treat points as euclidean vectors since they're basically the same
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct vector_t {
	int x;
	int y;
	int z; // Not used currently
};

struct polygon_t {
	struct vector_t* points;
	int num_points;
};

/**
 * Computes \f$(\pmb{v_1} \dot \pmb{v_2})\f$
 */
int dot(struct vector_t v1, struct vector_t v2);

/**
 * Computes \f$(\pmb{v_1} - \pmb{v_2})\f$
 */
struct vector_t sub(struct vector_t v1, struct vector_t v2);


/**
 * Computes \f$(s  \pmb{v})\f$ with scalar, s, and vector, v
 */
struct vector_t scalar_mult(int s, struct vector_t v);

/**
 * Computes (v1 x v2 x v3) using the following identity:
 *
 * \f$ (\pmb{v_1} x \pmb{v_2}) x \pmb{v_3} = (\pmb{v_1} \dot \pmb{v_3}) \pmb{v_2} - (\pmb{v_3} \dot \pmb{v_2}) \pmb{v_1} \f$
 *
 */
struct vector_t triple_product2(struct vector_t v1, struct vector_t v2, struct vector_t v3);

/**
 * Returns the integer square root of a number
 * int_sqrt = floor(sqrt(s))
 *
 * Copied from https://en.wikipedia.org/wiki/Integer_square_root#Example_implementation_in_C
 */
unsigned int int_sqrt(unsigned int s);

/**
 * Normalizes vector only if dot(v, v) != 0
 * \f$ \pmb{n} = \frac{\pmb{v}{|\pmb{v}|} = \frac{\pmb{v}}{\sqrt{\sum_i v_i^2}} \f$
 * 
 * @return normalized vector if dot(v, v) !=0 or just the vector if dot(v, v)==0
 */
struct vector_t normalize_if_norm_nonzero(struct vector_t v);

/**
 * Computes the centroid of a polygon by averaging over the number of points
 *
 * \f$ \bar{x} = \frac{\sum x_i}{N} \f$
 */
struct vector_t get_centroid(struct polygon_t poly);

#ifdef __cplusplus
}
#endif

#endif
