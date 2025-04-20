#ifndef NETWORK_HERO_H
#define NETWORK_HERO_H


#include "hero.h"


typedef struct {
	Vector2 position;
	Direction facing;
	HeroPalette palette;
} NetworkHeroState;


typedef struct {
	size_t owner;
	double last_synced;
	HeroHusk husk;
	NetworkHeroState target;
	NetworkHeroState previous;
} NetworkHero;


void hero_husk_get_state(HeroHusk* hero, NetworkHeroState* state);
void network_hero_set_state(NetworkHero* hero, NetworkHeroState* state);

void network_hero_handle_sync(NetworkHero* hero, NetworkHeroState* state);
void network_hero_handle_action(NetworkHero* hero);
void network_hero_update(NetworkHero* hero);
void network_hero_render(NetworkHero* hero);

#endif // NETWORK_HERO_H
