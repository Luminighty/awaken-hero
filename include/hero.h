#ifndef HERO_H
#define HERO_H

#include <raylib.h>


typedef enum {
	DIR_DOWN,
	DIR_UP,
	DIR_LEFT,
	DIR_RIGHT,
} Direction;

typedef struct {
	Rectangle position;
	int tile_x, tile_y;
	Direction facing;
} Hero;


Hero hero_create();
void hero_render(Hero* hero);
void hero_update(Hero* hero);


#endif // HERO_H
