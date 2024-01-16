#include "utils.h"

/**
 * Convert int* points into polygon_t type used by the gjk_collision function
 */
void convert_to_polygon_t(int *points, int num_points, struct polygon_t* polygon) {
	for (int i = 0; i < num_points; i++) {
		polygon->points[i] = (struct vector_t) {points[2*i], points[2*i+1]};
	}
}

/**
 * Convert int* points into x_arr and y_arr used by the sdl2 gfx functions
 */
void convert_to_sdl_arr(int* points, int num_points, int16_t* x_arr, int16_t* y_arr) {
	for (int i = 0; i < num_points; i++) {
		x_arr[i] = points[2*i];
		y_arr[i] = points[2*i+1];
	}
}
