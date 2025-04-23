#include "textures.h"
#include "config.h"
#include "hero.h"
#include <raylib.h>

Textures textures = {0};


void textures_create() {
	textures.render_target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	textures.hero = LoadTexture("./assets/link.png");
	textures.sword = LoadTexture("./assets/sword.png");
	textures.hero_palettes[HERO_PALETTE_GREEN] = LoadTexture("./assets/link.png");
	textures.hero_palettes[HERO_PALETTE_RED] = LoadTexture("./assets/link_red.png");
	textures.hero_palettes[HERO_PALETTE_BLUE] = LoadTexture("./assets/link_blue.png");
	textures.hero_palettes[HERO_PALETTE_PURPLE] = LoadTexture("./assets/link_purple.png");
	textures.tileset = LoadTexture("./assets/tileset.png");
}

void textures_destroy() {
	UnloadRenderTexture(textures.render_target);
	UnloadTexture(textures.hero);
	UnloadTexture(textures.tileset);
}
