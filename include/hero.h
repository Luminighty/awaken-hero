#ifndef HERO_H
#define HERO_H

#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include "collision.h"

typedef enum {
	DIR_DOWN,
	DIR_UP,
	DIR_LEFT,
	DIR_RIGHT,
} Direction;


typedef enum {
	HERO_STATE_IDLE,
	HERO_STATE_WALK,
	HERO_STATE_SWING,
} HeroState;


typedef enum {
	HERO_PALETTE_GREEN,
	HERO_PALETTE_RED,
	HERO_PALETTE_BLUE,
	HERO_PALETTE_PURPLE,
	HERO_PALETTE_SIZE,
} HeroPalette;


typedef struct {
	HeroPalette palette;
	HeroState state;
	bool flipped;
	bool is_moving;
	float flip_tick;
	float walk_tick;
} HeroAnimState;


typedef struct {
	size_t owner;
	Rectangle position;
	Direction facing;
	HeroAnimState animation;
	bool swinging;
	float swing_tick;
} HeroHusk;


typedef struct {
	HeroHusk husk;
	ColliderId collider;
	int tile_x, tile_y;
} Hero;


Hero hero_create();
void hero_render(HeroHusk* hero);
void hero_update(Hero* hero);
void hero_husk_update(HeroHusk* hero);


#endif // HERO_H
