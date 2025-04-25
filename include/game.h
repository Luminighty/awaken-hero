#ifndef GAME_H
#define GAME_H

#include "hero.h"
#include "map.h"
#include "network_hero.h"

#include <raylib.h>

#define MAX_HERO_HUSKS 4


typedef struct {
	Camera2D camera;
	Map map;
	Hero hero;
	NetworkHero network_heroes[MAX_HERO_HUSKS];
	size_t network_hero_count;
} Game;


extern Game game;

void game_create();
void game_destroy();
void game_update();
void game_render();
bool game_is_running();


#endif // GAME_H
