#include "hero.h"

#include "config.h"
#include "entity.h"
#include "log.h"
#include "textures.h"
#include "collision.h"
#include "client.h"
#include "utils.h"
#include "owl.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <stdio.h>


#define LOG_HEADER "HERO"

static const Vector2 size = { 10, 10 };
static const Vector2 origin = { 3, 6 };
const float HERO_SPEED = 80;
static const float WALK_DELAY = 0.4f;
static const float FLIP_DELAY = 0.2f;
static const float SWING_DELAY = 0.3f;
static const Vector2 ROOM_OFFSET = (Vector2){.x = 5.f, .y = 0.f };
#define RAYCAST(_x, _y, _dx, _dy) (Raycast){ \
	.origin = (Vector2){.x = _x, .y = _y}, \
	.point = (Vector2){.x = _dx, .y = _dy} \
}
static const Raycast USE_RAYCAST[][2] = {
	[DIR_DOWN] = { RAYCAST(0, size.y, 0, 4), RAYCAST(size.x, size.y, 0, 4) },
	[DIR_UP] = { RAYCAST(0, 0, 0, -4), RAYCAST(size.x, 0, 0, -4) },
	[DIR_LEFT] = { RAYCAST(0, 0, -4, 0), RAYCAST(0, size.y, -4, 0) },
	[DIR_RIGHT] = { RAYCAST(size.x, 0, 4, 0), RAYCAST(size.x, size.y, 4, 0) },
};

#undef RAYCAST


Hero hero_create() {
	Hero hero = {0};
	hero.id = entity_create_id(ENTITY_HERO);
	hero.husk.position = (Rectangle){.x = 83, .y = 83, .width = TILE_SIZE, .height = TILE_SIZE};
	hero.husk.facing = DIR_DOWN;
	Rectangle rect = (Rectangle){
		.x = hero.husk.position.x,
		.y = hero.husk.position.y,
		.width = size.x,
		.height = size.y,
	};
	hero.collider = collider_create(hero.id, rect, COLLISION_LAYER_PLAYER);
	hero.husk.animation.palette = HERO_PALETTE_PURPLE;
	collider_set_debug(hero.collider, true);
	return hero;
}

static void hero_update_animation(HeroHusk* hero, float dt);

static inline bool hero_can_walk(Hero* hero) {
	return !hero->husk.swinging;
}


#define TO_TILE(pos) pos / TILE_SIZE


static void handle_use(Hero* hero);

void hero_update(Hero* hero) {
	assert(hero);
	float dt = GetFrameTime();
	Vector2 input = {0};
	if (hero_can_walk(hero)) {
		if (IsKeyDown(INPUT_LEFT)) {
			input.x = -1;
			hero->husk.facing = DIR_LEFT;
		}
		if (IsKeyDown(INPUT_RIGHT)) {
			input.x = 1;
			hero->husk.facing = DIR_RIGHT;
		}
		if (IsKeyDown(INPUT_DOWN)) {
			input.y = 1;
			hero->husk.facing = DIR_DOWN;
		}
		if (IsKeyDown(INPUT_UP)) {
			input.y = -1;
			hero->husk.facing = DIR_UP;
		}
		Vector2 velocity = Vector2Scale(Vector2Normalize(input), HERO_SPEED * dt);
		collider_move(hero->collider, (Vector2){.x = velocity.x});
		Vector2 new_position = collider_move(hero->collider, (Vector2){.y = velocity.y});
		hero->husk.position.x = new_position.x;
		hero->husk.position.y = new_position.y;
		hero->room_x = (ROOM_OFFSET.x + new_position.x) / TILE_SIZE / ROOM_WIDTH;
		hero->room_y = (ROOM_OFFSET.y + new_position.y) / TILE_SIZE / ROOM_HEIGHT;
//		game.camera.target.x = hero->room_x * ROOM_WIDTH * TILE_SIZE;
//		game.camera.target.y = hero->room_y * ROOM_HEIGHT * TILE_SIZE;
	}
	if (!hero->husk.swinging && IsKeyPressed(INPUT_SWING)) {
		hero->husk.swinging = true;
		hero->husk.swing_tick = 0.f;
		message_send_action((MessageAction){.action = ACTION_SWING});
	}

	if (IsKeyPressed(INPUT_USE))
		handle_use(hero);
	hero->husk.animation.is_moving = fabs(input.x) + fabs(input.y) > 0.1f;
	hero_husk_update(&hero->husk);
}

static void handle_use(Hero* hero) {
	static const CollisionLayer layer = COLLISION_LAYER_OWL;
	RaycastHitResult result = {0};
	bool found = false;
	size_t cast_amount = sizeof(USE_RAYCAST[0]) / sizeof(USE_RAYCAST[0][0]);
	for (size_t i = 0; i < cast_amount && !found; i++) {
		Raycast cast = USE_RAYCAST[hero->husk.facing][i];
		cast.origin = Vector2Add(cast.origin, AS_VEC(hero->husk.position));
		found = collider_raycast_hit(cast, layer, &result);
	}

	if (!found) {
		LOG("Hit nothing\n");
		return;
	}
	switch (result.parent.type) {
	case ENTITY_OWL:
		owl_on_interact(entity_lookup(result.parent));
		return;
	default:
		LOG("Hit %s\n", ENTITY_TYPE_TO_STR[result.parent.type]);
		return;
	}
}

void hero_husk_update(HeroHusk *hero) {
	assert(hero);
	float dt = GetFrameTime();
	if (hero->swinging) {
		hero->swing_tick += dt;
		hero->swinging = hero->swing_tick < SWING_DELAY;
	}
	hero_update_animation(hero, dt);
}


static inline void hero_update_animation(HeroHusk* hero, float dt) {
	if (hero->swinging) {
		hero->animation.state = HERO_STATE_SWING;
		return;
	}

	hero->animation.state = hero->animation.is_moving ? HERO_STATE_WALK : HERO_STATE_IDLE;
	if (hero->animation.is_moving) {
		hero->animation.flip_tick += dt;
		hero->animation.walk_tick += dt;
		if (hero->animation.flip_tick > FLIP_DELAY) {
			hero->animation.flipped = !hero->animation.flipped;
			hero->animation.flip_tick -= FLIP_DELAY;
		}
		if (hero->animation.walk_tick > WALK_DELAY) {
			hero->animation.walk_tick -= WALK_DELAY;
		}
	} else {
		hero->animation.flip_tick = FLIP_DELAY;
		hero->animation.walk_tick = WALK_DELAY / 0.15f;
	}
}


#define RECT(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE + 1), .width = TILE_SIZE, .height = TILE_SIZE })
#define RECTF(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE + 1), .width = -TILE_SIZE, .height = TILE_SIZE })
#define NONE_SPRITE ((Rectangle){ .x = 7 * (TILE_SIZE + 1), .y = 0 * (TILE_SIZE + 1), .width = -TILE_SIZE, .height = -TILE_SIZE })

static inline Rectangle hero_sprite_idle(HeroHusk* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return hero->animation.flipped ? RECTF(0, 0) : RECT(0, 0);
	case DIR_UP:
		return hero->animation.flipped ? RECTF(1, 0) : RECT(1, 0);
	case DIR_LEFT:
		return RECT(2, 0);
	case DIR_RIGHT:
		return RECTF(2, 0);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


static inline Rectangle hero_sprite_walk(HeroHusk* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return hero->animation.flipped ? RECTF(0, 0) : RECT(0, 0);
	case DIR_UP:
		return hero->animation.flipped ? RECTF(1, 0) : RECT(1, 0);
	case DIR_LEFT:
		return hero->animation.walk_tick < WALK_DELAY / 2.f ? RECT(2, 0) : RECT(3, 0);
	case DIR_RIGHT:
		return hero->animation.walk_tick < WALK_DELAY / 2.f ? RECTF(2, 0) : RECTF(3, 0);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


static inline Rectangle hero_sprite_swing(HeroHusk* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return hero->swing_tick < SWING_DELAY / 2.f ? RECT(0, 1) : RECT(1, 1);
	case DIR_UP:
		return hero->swing_tick < SWING_DELAY / 2.f ? RECT(2, 1) : RECT(3, 1);
	case DIR_LEFT:
		return hero->swing_tick < SWING_DELAY / 2.f ? RECT(4, 1) : RECT(5, 1);
	case DIR_RIGHT:
		return hero->swing_tick < SWING_DELAY / 2.f ? RECTF(4, 1) : RECTF(5, 1);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


static Rectangle hero_sprite(HeroHusk* hero) {
	switch (hero->animation.state) {
	case HERO_STATE_IDLE:
		return hero_sprite_idle(hero);
	case HERO_STATE_WALK:
		return hero_sprite_walk(hero);
	case HERO_STATE_SWING:
		return hero_sprite_swing(hero);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


void hero_render(HeroHusk* hero) {
	assert(hero->position.width > 0 && hero->position.height > 0);
	DrawTexturePro(
		textures.hero_palettes[hero->animation.palette], 
		hero_sprite(hero),
		hero->position,
		origin,
		0,
		WHITE
	);
	draw_point(hero->position.x, hero->position.y, GREEN);

	size_t cast_amount = sizeof(USE_RAYCAST[0]) / sizeof(USE_RAYCAST[0][0]);
	for (size_t i = 0; i < cast_amount; i++) {
		Raycast cast = USE_RAYCAST[hero->facing][i];
		cast.origin = Vector2Add(cast.origin, AS_VEC(hero->position));
		Vector2 to = Vector2Add(cast.origin, cast.point);
		DrawLineEx(cast.origin, to, 1, RED);
	}
}

