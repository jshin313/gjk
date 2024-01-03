// Based on https://web.dev/articles/drawing-to-canvas-in-emscripten
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "gjk.h"

SDL_Window *window;
SDL_Renderer *renderer;

struct sdl_poly_t {
	int16_t* x_arr;
	int16_t* y_arr;
	int size;
};

struct sdl_poly_t sdl_poly1 = {
	.x_arr = (int16_t[]){10, 10, 70, 70}, 
	.y_arr = (int16_t[]){30, 60, 60, 30}, 
	.size=4
};

struct sdl_poly_t sdl_poly2 = {
	.x_arr = (int16_t []){100, 100 - 30, 100 - 15, 100 + 15, 100 + 30},
	.y_arr = (int16_t []){100 - 30, 100 - 10, 100 + 30, 100 + 30, 100 - 10},
	.size=5
};

// Will hold same information as above except in the format used by gjk_collision 
struct polygon_t gjk_poly1;
struct polygon_t gjk_poly2;

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

	polygonRGBA(renderer, sdl_poly1.x_arr, sdl_poly1.y_arr, sdl_poly1.size, color_r, color_g, color_b, 0xFF);
	polygonRGBA(renderer, sdl_poly2.x_arr, sdl_poly2.y_arr, sdl_poly2.size, color_r, color_g, color_b, 0xFF);

	SDL_RenderPresent(renderer);
}

// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
int pnpoly(int nvert, int16_t *vertx, int16_t *verty, int testx, int testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
     (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
}

/**
 * Convert SDLpoly_t type poly into polygon_t type used by the gjk_collision function
 */
void convert_to_polygon(struct sdl_poly_t poly, struct polygon_t* polygon) {
	for (int i = 0; i < poly.size; i++) {
		polygon->points[i] = (struct vector_t) {poly.x_arr[i], poly.y_arr[i], 0};
	}
}

bool handle_events() {
	static bool mouse_button_held = false;
	static bool sdl_poly1_selected = false;
	static bool sdl_poly2_selected = false;

	static uint32_t ticksForNextRedraw = 0;

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
		uint32_t ticksNow = SDL_GetTicks();
		if (SDL_TICKS_PASSED(ticksNow, ticksForNextRedraw)) {
			// Throttle redraw
			ticksForNextRedraw = ticksNow + 10;


			// Select which polygon to drag if you hold down mouse button and you click inside border of that polygon
			if (pnpoly(sdl_poly1.size, sdl_poly1.x_arr, sdl_poly1.y_arr, mouse_x, mouse_y) && !sdl_poly2_selected) {
				sdl_poly1_selected = true;
			} else if (pnpoly(sdl_poly2.size, sdl_poly2.x_arr, sdl_poly2.y_arr, mouse_x, mouse_y) && !sdl_poly1_selected) {
				sdl_poly2_selected = true;
			}

		}
	}
	// Update polygon state if you move mouse
	if (sdl_poly1_selected) {

		sdl_poly1.x_arr[0] = mouse_x-30;
		sdl_poly1.x_arr[1] = mouse_x-30;
		sdl_poly1.x_arr[2] = mouse_x+30;
		sdl_poly1.x_arr[3] = mouse_x+30;

		sdl_poly1.y_arr[0] = mouse_y - 15;
		sdl_poly1.y_arr[1] = mouse_y + 15;
		sdl_poly1.y_arr[2] = mouse_y + 15;
		sdl_poly1.y_arr[3] = mouse_y - 15;

		convert_to_polygon(sdl_poly1, &gjk_poly1);
		convert_to_polygon(sdl_poly2, &gjk_poly2);
		redraw(gjk_collision(gjk_poly1, gjk_poly2));

	} else if (sdl_poly2_selected) {

		sdl_poly2.x_arr[0] = mouse_x;
		sdl_poly2.x_arr[1] = mouse_x-30;
		sdl_poly2.x_arr[2] = mouse_x-15;
		sdl_poly2.x_arr[3] = mouse_x+15;
		sdl_poly2.x_arr[4] = mouse_x+30;

		sdl_poly2.y_arr[0] = mouse_y - 30;
		sdl_poly2.y_arr[1] = mouse_y - 10;
		sdl_poly2.y_arr[2] = mouse_y + 30;
		sdl_poly2.y_arr[3] = mouse_y + 30;
		sdl_poly2.y_arr[4] = mouse_y - 10;

		convert_to_polygon(sdl_poly1, &gjk_poly1);
		convert_to_polygon(sdl_poly2, &gjk_poly2);
		redraw(gjk_collision(gjk_poly1, gjk_poly2));
	}

	return true;
}

void call_handle_events(void) { 
	handle_events(); 
}

void run_main_loop() {
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(call_handle_events, 0, true);
#else
	while (handle_events())
		;
#endif
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer(320, 240, 0, &window, &renderer);

	gjk_poly1.points = (struct vector_t*) alloca(sdl_poly1.size * sizeof(struct vector_t));
	gjk_poly1.num_points = sdl_poly1.size;
	gjk_poly2.points = (struct vector_t*) alloca(sdl_poly2.size * sizeof(struct vector_t));
	gjk_poly2.num_points = sdl_poly2.size;

	convert_to_polygon(sdl_poly1, &gjk_poly1);
	convert_to_polygon(sdl_poly2, &gjk_poly2);

	redraw(gjk_collision(gjk_poly1, gjk_poly2));

	run_main_loop();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

