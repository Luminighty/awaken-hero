#include "collision.h"
#include "game.h"
#include <raylib.h>
#include <stdio.h>


int main() {
	printf("FENCE_SIZE: %lu", sizeof(Fence));
	game_create();
	while(game_is_running()) {
		game_update();
		game_render();
	}
	game_destroy();
}

