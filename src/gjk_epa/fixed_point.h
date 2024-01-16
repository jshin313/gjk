#include <stdint.h>
#include "vector.h"

#define FIXED_POINT_SCALING_FACTOR (1 << 8)
/* #define FIXED_POINT_SCALING_FACTOR 1000 */

// Fixed point type with 8 bits for fractional part
// scaling factor is 1/(2^8) = 1/256
typedef int64_t fixed_point_Q8_t;

fixed_point_Q8_t int_to_fixed_point(int64_t i);

void polygon_t_int_to_fixed_point(struct polygon_t P);

int64_t get_remainder(fixed_point_Q8_t f);

/** Truncates the fractional part
 * @return floor(f)
 */
int64_t fixed_point_to_int(fixed_point_Q8_t f);
