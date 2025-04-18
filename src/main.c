#include "game.h"
#include <raylib.h>


int main() {
	game_create();
	while(game_is_running()) {
		game_update();
		game_render();
	}
	game_destroy();
}
