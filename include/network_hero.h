#ifndef NETWORK_HERO_H
#define NETWORK_HERO_H

#include "entity.h"
#include "hero.h"


typedef struct {
	Vector2 position;
	Direction facing;
	HeroPalette palette;
} NetworkHeroState;


typedef struct {
	EntityId id;
	size_t owner;
	double last_synced;
	HeroHusk husk;
	NetworkHeroState target;
	NetworkHeroState previous;
} NetworkHero;


void hero_husk_get_state(HeroHusk* hero, NetworkHeroState* state);
void network_hero_init(NetworkHero* hero, NetworkHeroState* state);

void network_hero_handle_sync(NetworkHero* hero, NetworkHeroState* state);
void network_hero_handle_action(NetworkHero* hero, float x, float y);
void network_hero_update(NetworkHero* hero);
void network_hero_render(NetworkHero* hero);


#endif // NETWORK_HERO_H
