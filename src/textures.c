#include "textures.h"
#include "config.h"
#include "hero.h"
#include <raylib.h>

Textures textures = {0};


#define TEXTURES_REGISTRY \
	X(textures.hero, "./assets/link.png")\
	X(textures.sword, "./assets/sword.png")\
	X(textures.hero_palettes[HERO_PALETTE_GREEN], "./assets/link.png")\
	X(textures.hero_palettes[HERO_PALETTE_RED], "./assets/link_red.png")\
	X(textures.hero_palettes[HERO_PALETTE_BLUE], "./assets/link_blue.png")\
	X(textures.hero_palettes[HERO_PALETTE_PURPLE], "./assets/link_purple.png")\
	X(textures.tileset, "./assets/tileset.png")\


void textures_create() {
	textures.render_target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

	#define X(field, file) (field) = LoadTexture(file);
	TEXTURES_REGISTRY
	#undef X
}


void textures_destroy() {
	#define X(field, file) UnloadTexture(field);
		TEXTURES_REGISTRY
	#undef X

	UnloadRenderTexture(textures.render_target);
}

