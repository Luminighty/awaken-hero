#include "network_hero.h"
#include "client.h"
#include "hero.h"
#include "log.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "config.h"

#define LOG_HEADER "NETWORK_HERO"

void hero_husk_get_state(HeroHusk* hero, NetworkHeroState* state) {
	state->position.x = hero->position.x;
	state->position.y = hero->position.y;
	state->facing = hero->facing;
	state->palette = hero->animation.palette;
}

void network_hero_set_state(NetworkHero* hero, NetworkHeroState* state) {
	hero->husk.position.x = state->position.x;
	hero->husk.position.y = state->position.y;
	hero->husk.position.width = TILE_SIZE;
	hero->husk.position.height = TILE_SIZE;
	hero->husk.facing = state->facing;
	hero->husk.animation.palette = state->palette;
	hero->previous = *state;
	hero->target = *state;
}

void network_hero_handle_sync(NetworkHero* hero, NetworkHeroState* state) {
	hero->previous = hero->target;
	hero->target = *state;
	hero->last_synced = GetTime();
	hero->husk.animation.palette = state->palette;
	hero->husk.facing = state->facing;
}

void network_hero_handle_action(NetworkHero* hero) {
	// TODO: Do action based on message
	LOG("Hero %zu action received.\n", hero->owner);
	hero->husk.swinging = true;
	hero->husk.swing_tick = 0.f;
}


void network_hero_update(NetworkHero* hero) {
	// TODO: Interpolate data
	float dt = GetFrameTime();
	float time = GetTime();
	float progress = (time - hero->last_synced) / NETWORK_SYNC_DELAY;


	Vector2 position = (Vector2){.x = hero->husk.position.x, .y = hero->husk.position.y};
	Vector2 input = Vector2Subtract(hero->target.position, hero->previous.position);
	Vector2 target_position = Vector2Add(hero->previous.position, Vector2Scale(input, progress));
	float distance = Vector2LengthSqr(input);
	if (distance > 10.f) {
		Vector2 delta = Vector2Subtract(target_position, position);
		Vector2 velocity = Vector2Scale(Vector2Normalize(delta), HERO_SPEED * dt);
		position = Vector2Add(position, velocity);
		hero->husk.position.x = position.x;
		hero->husk.position.y = position.y;
		hero->husk.animation.is_moving = true;
	} else {
		hero->husk.position.x = hero->target.position.x;
		hero->husk.position.y = hero->target.position.y;
		hero->husk.animation.is_moving = false;
	}
	
	hero_husk_update(&hero->husk);
}


void network_hero_render(NetworkHero* hero) {
	hero_render(&hero->husk);
}

