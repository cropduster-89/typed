#pragma once
#ifndef TYPED_EVENT_H
#define TYPED_EVENT_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

enum UPDATE_PHASE {
	UPHASE_IDLE,
	UPHASE_SCROLLOUT,
	UPHASE_SCROLLIN,
};

enum event_types {
	EVENT_NULL,
	
	EVENT_MOVEUP,
	EVENT_MOVEDOWN,
	EVENT_MOVELEFT,
	EVENT_BLINK,
	EVENT_FADE,
	EVENT_FLASH,
};

#define EVENT_CHECK_PROGRESS(name) bool name(union vec2 pos, union vec2 destination)
typedef EVENT_CHECK_PROGRESS(event_check_progress);

struct event_move {
	union vec2 destination;
	union vec2 increment;
	union vec2 origin;
	event_check_progress *CheckProgress;
};

struct event_flash {
	uint64_t duration;
	uint64_t interval;
	uint64_t lastFlash;
	uint64_t startTime;
};

struct event_blink {
	uint64_t start;		
};

struct event_fade {
	uint64_t start;		
	uint64_t end;		
};

struct entity_event {
	uint32_t index;
	uint32_t parentIndex;
	enum event_types type;
	union {
		struct event_fade fade;
		struct event_move move;
		struct event_blink blink;
		struct event_flash flash;
	};
};

struct game_state;
static struct entity_event *NewEvent(struct game_state *, int32_t, enum event_types);

#endif