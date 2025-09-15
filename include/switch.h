#ifndef SWITCH_H
#define SWITCH_H

#include "collision.h"
#include <raylib.h>


typedef struct {
	EntityId id;
	Rectangle position;
	ColliderId collider;
} Switch;


Switch switch_create(int x, int y);
void switch_destroy(Switch* _switch);
void switch_render(Switch* _switch);


#endif // SWITCH_H
