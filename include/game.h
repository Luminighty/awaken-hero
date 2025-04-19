#ifndef GAME_H
#define GAME_H

#include "hero.h"
#include "map.h"


#define MAX_HERO_HUSKS 4

typedef struct {
	Map map;
	Hero hero;
	HeroHusk hero_husks[MAX_HERO_HUSKS];
	size_t hero_husk_count;
} Game;

extern Game game;

void game_create();
void game_destroy();
void game_update();
void game_render();
bool game_is_running();

#endif // GAME_H
