#ifndef MESSAGE_H
#define MESSAGE_H

#include "network_hero.h"
#include <stddef.h>


typedef enum {
	TAG__NONE = 0,
	TAG_CONNECT,
	TAG_DISCONNECT,
	TAG_ASSIGN_UID,
	TAG_SYNC_STATE,
	TAG_ACTION,
	TAG__SIZE,
} MessageTag;


typedef enum {
	ACTION_SWING,
	ACTION_FALL,
	ACTION_SET_POSITION,
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


extern const char* message_tag_labels[TAG__SIZE];


#endif // MESSAGE_H
