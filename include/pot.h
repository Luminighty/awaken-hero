#ifndef POT_H
#define POT_H

#include "collision.h"
#include <raylib.h>


typedef struct {
	EntityId id;
	Rectangle position;
	ColliderId collider;
} Pot;


Pot pot_create(int x, int y);
void pot_destroy(Pot* pot);
void pot_render(Pot* pot);


#endif // POT_H
