#include "audio.h"
#include "random.h"
#include <raylib.h>
#include "utils.h"

Audios audios = {0};


#define SOUND_REGISTRY \
	X(audios.sword[0], "./assets/sounds/sword_1.wav")\
	X(audios.sword[1], "./assets/sounds/sword_2.wav")\
	X(audios.sword[2], "./assets/sounds/sword_3.wav")\
	X(audios.sword[3], "./assets/sounds/sword_4.wav")\
	X(audios.open, "./assets/sounds/open.wav")\
	X(audios.bomb, "./assets/sounds/bomb.wav")\
	X(audios.boots, "./assets/sounds/boots.wav")\
	X(audios.death, "./assets/sounds/death.wav")\
	X(audios.door, "./assets/sounds/door.wav")\
	X(audios.enemy_kill, "./assets/sounds/enemy_kill.wav")\
	X(audios.enemy_hit, "./assets/sounds/enemy_hit.wav")\
	X(audios.water_enter, "./assets/sounds/water_enter.wav")\
	X(audios.fall, "./assets/sounds/fall.wav")\
	X(audios.heart, "./assets/sounds/heart.wav")\
	X(audios.hookshot, "./assets/sounds/hookshot.wav")\
	X(audios.item_get, "./assets/sounds/item_get.wav")\
	X(audios.item_important, "./assets/sounds/item_important.wav")\
	X(audios.item_normal, "./assets/sounds/item_normal.wav")\
	X(audios.jump, "./assets/sounds/jump.wav")\
	X(audios.owl, "./assets/sounds/owl.wav")\
	X(audios.power_lift, "./assets/sounds/power_lift.wav")\
	X(audios.shield, "./assets/sounds/shield.wav")\
	X(audios.sound_open, "./assets/sounds/sound_open.wav")\
	X(audios.stairs, "./assets/sounds/stairs.wav")\
	X(audios.swim, "./assets/sounds/swim.wav")\
	X(audios.treasure_big, "./assets/sounds/treasure_big.wav")\
	X(audios.treasure_hidden, "./assets/sounds/treasure_hidden.wav")\
	X(audios.treasure_small, "./assets/sounds/treasure_small.wav")\


#define MUSIC_REGISTRY \
	X(audios.level_01, "./assets/musics/level_01.mp3")\


void audios_create() {
	#define X(field, file) field = LoadMusicStream(file);
		MUSIC_REGISTRY
	#undef X
	#define X(field, file) field = LoadSound(file);
		SOUND_REGISTRY
	#undef X
}


void audios_destroy() {
	#define X(field, file) UnloadMusicStream(field);
		MUSIC_REGISTRY
	#undef X
	#define X(field, file) UnloadSound(field);
		SOUND_REGISTRY
	#undef X
}


void sound_play(SoundEffect sound) {
	switch (sound) {
	case SOUND_SWORD:	PlaySound(random_array(audios.sword)); break;
	case SOUND_OPEN:	PlaySound(audios.open); break;
	case SOUND_BOMB:	PlaySound(audios.bomb); break;
	case SOUND_BOOTS:	PlaySound(audios.boots); break;
	case SOUND_DEATH:	PlaySound(audios.death); break;
	case SOUND_DOOR:	PlaySound(audios.door); break;
	case SOUND_ENEMY_KILL:	PlaySound(audios.enemy_kill); break;
	case SOUND_ENEMY_HIT:	PlaySound(audios.enemy_hit); break;
	case SOUND_WATER_ENTER:	PlaySound(audios.water_enter); break;
	case SOUND_SWIM:	PlaySound(audios.swim); break;
	case SOUND_FALL:	PlaySound(audios.fall); break;
	case SOUND_HEART:	PlaySound(audios.heart); break;
	case SOUND_HOOKSHOT:	PlaySound(audios.hookshot); break;
	case SOUND_JUMP:	PlaySound(audios.jump); break;
	case SOUND_OWL: 	PlaySound(audios.owl); break;
	case SOUND_POWER_LIFT:	PlaySound(audios.power_lift); break;
	case SOUND_SHIELD:	PlaySound(audios.shield); break;
	case SOUND_SOUND_OPEN:	PlaySound(audios.sound_open); break;
	case SOUND_STAIRS:	PlaySound(audios.stairs); break;
	case SOUND_TREASURE_BIG:	PlaySound(audios.treasure_big); break;
	case SOUND_TREASURE_HIDDEN:	PlaySound(audios.treasure_hidden); break;
	case SOUND_TREASURE_SMALL:	PlaySound(audios.treasure_small); break;
	case SOUND_ITEM_GET:		PlaySound(audios.item_get); break;
	case SOUND_ITEM_IMPORTANT:	PlaySound(audios.item_important); break;
	case SOUND_ITEM_NORMAL:		PlaySound(audios.item_normal); break;
	}
}

