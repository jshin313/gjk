#ifdef TI84PCE

#include <graphx.h>
#include "loop_ti84pce.h"

#else

#include <SDL2/SDL.h>
#include "loop.h"

#endif

int main(void) {
#ifdef TI84PCE
	gfx_Begin();
	gfx_SetDrawBuffer();

	run_main_loop();

	gfx_End();
#else
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(320, 240, 0, &window, &renderer);

	run_main_loop();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
#endif
}
