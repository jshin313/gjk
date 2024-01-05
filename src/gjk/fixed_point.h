#include <stdint.h>
#include "vector.h"

// Fixed point type with 4 bits for fractional part
// scaling factor is 1/(2^16) = 1/65535
typedef int64_t fixed_point_Q16_t;

fixed_point_Q16_t int_to_fixed_point(int64_t i);

void polygon_t_int_to_fixed_point(struct polygon_t P);

int64_t get_remainder(fixed_point_Q16_t f);

/** Truncates the fractional part
 * @return floor(f)
 */
int64_t fixed_point_to_int(fixed_point_Q16_t f);
