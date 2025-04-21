#ifndef UTILS_H
#define UTILS_H


#include <raylib.h>

#define assert_array_push(array, count) assert(sizeof(array) / sizeof((array)[0]) > (size_t)(count))

void draw_point(int x, int y, Color color);

#define AS_VEC(vec) (Vector2){.x = (vec).x, .y = (vec).y}


#endif // UTILS_H
