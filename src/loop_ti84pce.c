#ifdef TI84PCE
#include <keypadc.h>
#include <graphx.h>
#include <alloca.h>

#include "gjk_epa/gjk.h"
#include "utils.h"

#include "loop_ti84pce.h"

const int GREEN = 0x07;
const int RED = 0xE0;

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

void redraw() {
	struct polygon_t gjk_poly1 = {
		.points = (struct vector_t*) alloca(NUM_POINTS_1 * sizeof(struct vector_t)),
		.num_points = NUM_POINTS_1,
	};
	struct polygon_t gjk_poly2 = {
		.points = (struct vector_t*) alloca(NUM_POINTS_2 * sizeof(struct vector_t)),
		.num_points = NUM_POINTS_2,
	};
	convert_to_polygon_t(points1, gjk_poly1.num_points, &gjk_poly1);
	convert_to_polygon_t(points2, gjk_poly2.num_points, &gjk_poly2);

	int color = GREEN;
	if(gjk_collision(gjk_poly1, gjk_poly2, NULL)) {
		color = RED;
	}

	gfx_FillScreen(0xFF);
	gfx_SetColor(color);
	gfx_Polygon(points1, gjk_poly1.num_points);
	gfx_Polygon(points2, gjk_poly2.num_points);

	gfx_SwapDraw();
}

void run_main_loop()  {
	int x = 100, y = 100;
	do
	{
		kb_key_t arrows;

		/* Scan the keypad to update kb_Data */
		kb_Scan();

		/* Get the arrow key statuses */
		arrows = kb_Data[7];

		/* Check if any arrows are pressed */
		if (arrows)
		{
			int dx = 3, dy = 3;
			/* Do different directions depending on the keypress */
			if (arrows & kb_Right)
			{
				x += dx;
			}
			if (arrows & kb_Left)
			{
				x -= dx;
			}
			if (arrows & kb_Down)
			{
				y += dy;
			}
			if (arrows & kb_Up)
			{
				y -= dy;
			}
		}

		/* Update Polygon 2 */
		points2[0] = x,    points2[1] = y-30;  // (x0, y0)
		points2[2] = x-30, points2[3] = y-10;  // (x1, y1)
		points2[4] = x-15, points2[5] = y+30;  // (x2, y2)
		points2[6] = x+15, points2[7] = y+30;  // (x3, y3)
		points2[8] = x+30, points2[9] = y-10;  // (x4, y4)

		redraw();

	} while (kb_Data[6] != kb_Clear);
}

#endif
