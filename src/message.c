#include "message.h"


const char* message_tag_labels[TAG__SIZE] = {
	[TAG__NONE] = "TAG__NONE",
	[TAG_CONNECT] = "TAG_CONNECT",
	[TAG_DISCONNECT] = "TAG_DISCONNECT",
	[TAG_ASSIGN_UID] = "TAG_ASSIGN_UID",
	[TAG_SYNC_STATE] = "TAG_SYNC_STATE",
	[TAG_ACTION] = "TAG_ACTION",
};

