#ifndef EPA_H
#define EPA_H

#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOLERANCE 1

/**
 * @return penetration vector with information on depth and direction of collision
 */
struct vector_t epa(struct polygon_t poly1, struct polygon_t poly2);

#ifdef __cplusplus
}
#endif

#endif
