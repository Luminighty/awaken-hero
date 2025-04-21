#ifndef CHEST_H
#define CHEST_H

#include "collision.h"
#include <raylib.h>


typedef struct {
	Rectangle position;
	ColliderId collider;
} Chest;

Chest chest_create(int x, int y);
void chest_destroy(Chest* chest);
void chest_render(Chest* chest);

#endif // CHEST_H
