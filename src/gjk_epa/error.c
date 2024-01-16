#include "error.h"
#include <string.h>

#define ARRAY_LEN(x) sizeof(x)/sizeof((x)[0])

const char* SIMPLEX_ERROR_STRS[] = 
{
    "ERROR: The simplex for GJK can only have a maximum of 3 points. Cannot add any more points.",
    "ERROR: The simplex has reached it's max capacity. Cannot add any more points.",
    "ERROR: The simplex is empty. No more points can be removed.",
    "SUCCESS: Note you should not see this string used.",
};
_Static_assert(ARRAY_LEN(SIMPLEX_ERROR_STRS) == NUM_SIMPLEX_ERROR_TYPES, "");

const char* simplex_error_string(enum simplex_error_t status) {
	if (status >= NUM_SIMPLEX_ERROR_TYPES) {
		return NULL;
	}

	return SIMPLEX_ERROR_STRS[status];
}

