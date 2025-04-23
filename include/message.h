#ifndef MESSAGE_H
#define MESSAGE_H

#include "network_hero.h"
#include <stddef.h>


typedef enum {
	TAG_NONE,
	TAG_CONNECT,
	TAG_DISCONNECT,
	TAG_ASSIGN_UID,
	TAG_SYNC_STATE,
	TAG_ACTION,
} MessageTag;


typedef enum {
	ACTION_SWING,
} Action;


typedef struct {
	Action action;
	float x, y;
	size_t owner;
} MessageAction;


typedef struct {
	NetworkHeroState network_hero;
} MessageState;


typedef struct {
	MessageTag tag;
	size_t sender;
	union {
		size_t uid;
		MessageState state;
		MessageAction action;
	} data;
} Message;


#endif // MESSAGE_H
