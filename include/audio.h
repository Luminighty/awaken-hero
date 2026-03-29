#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>


typedef enum {
	SOUND_SWORD,
	SOUND_OPEN,
	SOUND_BOMB,
	SOUND_BOOTS,
	SOUND_DEATH,
	SOUND_DOOR,
	SOUND_ENEMY_KILL,
	SOUND_ENEMY_HIT,
	SOUND_WATER_ENTER,
	SOUND_SWIM,
	SOUND_FALL,
	SOUND_HEART,
	SOUND_HOOKSHOT,
	SOUND_ITEM_GET,
	SOUND_ITEM_IMPORTANT,
	SOUND_ITEM_NORMAL,
	SOUND_JUMP,
	SOUND_OWL,
	SOUND_POWER_LIFT,
	SOUND_SHIELD,
	SOUND_SOUND_OPEN,
	SOUND_STAIRS,
	SOUND_TREASURE_BIG,
	SOUND_TREASURE_HIDDEN,
	SOUND_TREASURE_SMALL,
} SoundEffect;


typedef struct {
	Music level_01;

	Sound sword[4];
	Sound open;
	Sound bomb;
	Sound boots;
	Sound death;
	Sound door;
	Sound enemy_kill;
	Sound enemy_hit;
	Sound water_enter;
	Sound swim;
	Sound fall;
	Sound heart;
	Sound hookshot;
	Sound item_get;
	Sound item_important;
	Sound item_normal;
	Sound jump;
	Sound owl;
	Sound power_lift;
	Sound shield;
	Sound sound_open;
	Sound stairs;
	Sound treasure_big;
	Sound treasure_hidden;
	Sound treasure_small;
} Audios;


extern Audios audios;

void audios_create();
void audios_destroy();

void sound_play(SoundEffect sound);


#endif // AUDIO_H
