#include "hero.h"

#include "config.h"
#include "textures.h"
#include "collision.h"

#include <raylib.h>
#include <assert.h>
#include <stdio.h>

static const Vector2 size = { 10, 10 };
static const Vector2 origin = { 3, 6 };
static const float SPEED = 100;

Hero hero_create() {
	Hero hero = {0};
	hero.position = (Rectangle){.x = 83, .y = 83, .width = TILE_SIZE, .height = TILE_SIZE};
	hero.facing = DIR_DOWN;
	hero.tile_x = hero.position.x / 16;
	hero.tile_y = hero.position.y / 16;
	hero.collider = collider_create((Rectangle){
		.x = hero.position.x,
		.y = hero.position.y,
		.width = size.x,
		.height = size.y,
	});
	collider_set_debug(hero.collider, true);
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



#define TO_TILE(pos) pos / TILE_SIZE
void hero_update(Hero* hero) {
	assert(hero);
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
	collider_move(hero->collider, (Vector2){.x = delta.x});
	Vector2 new_position = collider_move(hero->collider, (Vector2){.y = delta.y});
	hero->position.x = new_position.x;
	hero->position.y = new_position.y;
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
