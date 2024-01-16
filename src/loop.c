#ifndef TI84PCE

// SDL2 rendering code based on https://web.dev/articles/drawing-to-canvas-in-emscripten
#include <stdbool.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "gjk/gjk.h"
#include "utils.h"
#include "loop.h"
#include "gjk/fixed_point.h"

// Set up polygons
int points1[] = {
	10+50, 30,   // (x0, y0)
	10+50, 60,   // (x1, y1)
	70+50, 60,   // (x2, y2)
	70+50, 30,   // (x3, y3)
};

int points2[] = {
	100,    100-30,   // (x0, y0)
	100-30, 100-10,   // (x1, y1)
	100-15, 100+30,   // (x2, y2)
	100+15, 100+30,   // (x3, y3)
	100+30, 100-10,   // (x4, y4)
};

// Divide by 2 since each point has is a pair
#define NUM_POINTS_1 sizeof(points1)/sizeof(points1[0])/2
#define NUM_POINTS_2 sizeof(points2)/sizeof(points2[0])/2

SDL_Window *window;
SDL_Renderer *renderer;

// These will the same as the points above, but they are in the format that gjk wants
void redraw(bool collision) {

	SDL_SetRenderDrawColor(renderer, /* RGBA: black */ 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	// GREEN
	uint8_t color_r = 0x00;
	uint8_t color_g = 0xFF;
	uint8_t color_b = 0x00;

	// RED
	if (collision) {
		color_r = 0xFF;
		color_g = 0x00;
		color_b = 0x00;
	} 

	int16_t sdl_x_arr_1[NUM_POINTS_1], sdl_y_arr_1[NUM_POINTS_1];
	int16_t sdl_x_arr_2[NUM_POINTS_2], sdl_y_arr_2[NUM_POINTS_2];
	convert_to_sdl_arr(points1, NUM_POINTS_1, sdl_x_arr_1, sdl_y_arr_1);
	convert_to_sdl_arr(points2, NUM_POINTS_2, sdl_x_arr_2, sdl_y_arr_2);

	polygonRGBA(renderer, sdl_x_arr_1, sdl_y_arr_1, NUM_POINTS_1, color_r, color_g, color_b, 0xFF);
	polygonRGBA(renderer, sdl_x_arr_2, sdl_y_arr_2, NUM_POINTS_2, color_r, color_g, color_b, 0xFF);

	SDL_RenderPresent(renderer);
}

// Modified from http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
// Uses int* points = (x0, y0, y1, y2, ...)
bool pnpoly(int num_points, int* points, int testx, int testy)
{
	int i, j, c = false;
	for (i = 0, j = num_points-1; i < num_points; j = i++) {
		int py_i = points[2*i+1]; // verty[i]
		int px_i = points[2*i];   // vertx[i]
		int py_j = points[2*j+1]; // verty[j]
		int px_j = points[2*j];   // vertx[j]

		if ( ((py_i>testy) != (py_j>testy)) &&
				(testx < (px_j-px_i) * (testy-py_i) / (py_j-py_i) + px_i))
			c = !c;
	}
	return c;
}

bool handle_events() {

	static bool mouse_button_held = false;
	static bool sdl_poly1_selected = false;
	static bool sdl_poly2_selected = false;

	SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		return false;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN) {
		mouse_button_held = true;
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		mouse_button_held = false;
		sdl_poly1_selected = false;
		sdl_poly2_selected = false;
	}

	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);

	if (mouse_button_held) {
		// Select which polygon to drag if you hold down mouse button and you click inside border of that polygon
		if (pnpoly(NUM_POINTS_1, points1, mouse_x, mouse_y) && !sdl_poly2_selected) {
			sdl_poly1_selected = true;
		} else if (pnpoly(NUM_POINTS_2, points2, mouse_x, mouse_y) && !sdl_poly1_selected) {
			sdl_poly2_selected = true;
		}
	}
	
	// Update polygon state if you move mouse
	if (sdl_poly1_selected) {
		points1[0] = mouse_x-30, points1[1] = mouse_y - 15;  // (x0, y0)
		points1[2] = mouse_x-30, points1[3] = mouse_y + 15;  // (x1, y1)
		points1[4] = mouse_x+30, points1[5] = mouse_y + 15;  // (x2, y2)
		points1[6] = mouse_x+30, points1[7] = mouse_y - 15;  // (x3, y3)
	} else if (sdl_poly2_selected) {                        
		points2[0] = mouse_x,    points2[1] = mouse_y-30;   // (x0, y0)
		points2[2] = mouse_x-30, points2[3] = mouse_y-10;   // (x1, y1)
		points2[4] = mouse_x-15, points2[5] = mouse_y+30;   // (x2, y2)
		points2[6] = mouse_x+15, points2[7] = mouse_y+30;   // (x3, y3)
		points2[8] = mouse_x+30, points2[9] = mouse_y-10;   // (x4, y4)
	}

	static uint32_t ticksForNextRedraw = 0;

	uint32_t ticksNow = SDL_GetTicks();
	if (SDL_TICKS_PASSED(ticksNow, ticksForNextRedraw)) {
		// Throttle redraw
		ticksForNextRedraw = ticksNow + 10;

		struct polygon_t gjk_poly1 = {
			.points = (struct vector_t*) alloca(NUM_POINTS_1 * sizeof(struct vector_t)),
			.num_points = NUM_POINTS_1,
		};
		struct polygon_t gjk_poly2 = {
			.points = (struct vector_t*) alloca(NUM_POINTS_2 * sizeof(struct vector_t)),
			.num_points = NUM_POINTS_2,
		};

		convert_to_polygon_t(points1, NUM_POINTS_1, &gjk_poly1);
		convert_to_polygon_t(points2, NUM_POINTS_2, &gjk_poly2);

		redraw(gjk_collision(gjk_poly1, gjk_poly2));
		struct vector_t penetration_vector = epa(gjk_poly1, gjk_poly2);
		printf("penetration vector: x: %lld + %ld/%d\n", fixed_point_to_int(penetration_vector.x), get_remainder(penetration_vector.x), FIXED_POINT_SCALING_FACTOR);
		printf("penetration vector: y: %lld + %ld/%d\n", fixed_point_to_int(penetration_vector.y), get_remainder(penetration_vector.y), FIXED_POINT_SCALING_FACTOR);
		puts("");
	}

	return true;
}

void call_handle_events(void) { 
	handle_events(); 
}

void run_main_loop(void) {
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(call_handle_events, 0, true);
#else
	while (handle_events())
		;
#endif
}
#endif
