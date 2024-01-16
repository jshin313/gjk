#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

#ifdef TI84PCE
#include <debug.h>
#define LOG(fmt, ...) dbg_printf((fmt), ##__VA_ARGS__)
#else
#define LOG(fmt, ...) fprintf(stderr, (fmt), ##__VA_ARGS__)
#endif

enum simplex_error_t {
	GJK_SIMPLEX_GREATER_THAN_3=0,
	SIMPLEX_REACHED_MAX_CAPACITY,
	EMPTY_SIMPLEX,
	SIMPLEX_SUCCESS,

	NUM_SIMPLEX_ERROR_TYPES // Not used as an actual error code, but only to keep count of errors
};

const char* simplex_error_string(enum simplex_error_t status);

#endif
