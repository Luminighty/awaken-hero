#ifndef GAME_H
#define GAME_H

#include "hero.h"
#include "map.h"


typedef struct {
	Map map;
	Hero hero;
} Game;

extern Game game;

void game_create();
void game_destroy();
void game_update();
void game_render();
bool game_is_running();

#endif // GAME_H
