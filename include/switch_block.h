#ifndef SWITCH_BLOCK_H
#define SWITCH_BLOCK_H

#include "collision.h"
#include <raylib.h>


typedef struct {
	EntityId id;
	Rectangle position;
	ColliderId collider;
	bool enabled;
} SwitchBlock;


SwitchBlock switch_block_create(int x, int y, bool enabled);
void switch_block_destroy(SwitchBlock* switch_block);
void switch_block_render(SwitchBlock* switch_block);

void switch_block_toggle_start(SwitchBlock* switch_block);
void switch_block_toggle_finish(SwitchBlock* switch_block);


#endif // SWITCH_BLOCK_H
