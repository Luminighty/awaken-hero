#include "utils.h"

static const int POINT_SIZE = 2;
void draw_point(int x, int y, Color color) {
	DrawLine(x - POINT_SIZE, y - POINT_SIZE, x + POINT_SIZE, y + POINT_SIZE, color);
	DrawLine(x + POINT_SIZE, y - POINT_SIZE, x - POINT_SIZE, y + POINT_SIZE, color);
}

