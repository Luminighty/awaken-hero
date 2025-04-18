#include "hero.h"
#include "config.h"
#include "textures.h"
#include <raylib.h>
#include <stdio.h>
#include "collision.h"

Hero hero_create() {
	Hero hero = {0};
	hero.position = (Rectangle){.x = 60, .y = 60, .width = TILE_SIZE, .height = TILE_SIZE};
	hero.tile_x = hero.position.x / 16;
	hero.tile_y = hero.position.y / 16;
	return hero;
}


#define RECT(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE - 1), .width = TILE_SIZE, .height = TILE_SIZE })
#define RECTF(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE - 1), .width = -TILE_SIZE, .height = TILE_SIZE })

static inline Rectangle hero_sprite(Hero* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return RECT(0, 0);
	case DIR_UP:
		return RECT(1, 0);
	case DIR_LEFT:
		return RECT(2, 0);
	case DIR_RIGHT:
		return RECTF(2, 0);
	default:
		return RECT(7, 0);
	}
}

static Vector2 origin = { TILE_SIZE / 2.f, TILE_SIZE};
static const float SPEED = 100;


#define TO_TILE(pos) pos / TILE_SIZE
void hero_update(Hero* hero) {
	float dt = GetFrameTime();
	Vector2 delta = {0};
	if (IsKeyDown(INPUT_LEFT)) {
		delta.x = -SPEED * dt;
		hero->facing = DIR_LEFT;
	}
	if (IsKeyDown(INPUT_RIGHT)) {
		delta.x = SPEED * dt;
		hero->facing = DIR_RIGHT;
	}
	if (IsKeyDown(INPUT_DOWN)) {
		delta.y = SPEED * dt;
		hero->facing = DIR_DOWN;
	}
	if (IsKeyDown(INPUT_UP)) {
		delta.y = -SPEED * dt;
		hero->facing = DIR_UP;
	}
	float new_x = hero->position.x + delta.x;
	int new_tile_x = new_x / TILE_SIZE;
	if (is_tile_solid(new_tile_x, hero->tile_y)) {
		new_x = hero->position.x;
		new_tile_x = hero->tile_x;
	}

	float new_y = hero->position.y + delta.y;
	int new_tile_y = new_y / TILE_SIZE;
	if (is_tile_solid(hero->tile_x, new_tile_y)) {
		new_y = hero->position.y;
		new_tile_y = hero->tile_y;
	}

	hero->position.x = new_x;
	hero->position.y = new_y;
	hero->tile_x = new_tile_x;
	hero->tile_y = new_tile_y;
}

void hero_render(Hero* hero) {
	DrawTexturePro(
		textures.hero, 
		hero_sprite(hero),
		hero->position,
		origin,
		0,
		WHITE
	);
}
