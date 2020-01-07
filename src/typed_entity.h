#pragma once
#ifndef TYPED_ENTITY_H
#define TYPED_ENTITY_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

enum entity_type {
	ENTTYPE_NULL,	
	
	ENTTYPE_BANNERRECT,
	ENTTYPE_OUTPUTCLIPRECT,
	ENTTYPE_UPDATESTRING,
	ENTTYPE_LABELSTRING,
	ENTTYPE_WPMSTRING,
	ENTTYPE_ACCSTRING,
	ENTTYPE_INPUTSTRING,
	ENTTYPE_OUTPUTSTRING,
	ENTTYPE_CURSORRECT,
	ENTTYPE_GENERICRECT,
	ENTTYPE_PROGRESSRECT,
	ENTTYPE_SCORELABEL,
};

/*
*	All entities that are created at startup
*	have a hardcoded index alias, all those
*	that are created dynamincally start
* 	from ENTALIAS_DYNAMICSTART
*/
enum entity_alias {
	ENTALIAS_BACKRECT,
	ENTALIAS_BANNERRECT,
	ENTALIAS_BANNERDECOR,
	ENTALIAS_BANNERNUB,
	ENTALIAS_BANNERUPDATE,
	ENTALIAS_INPUTRECTBOTTOM,
	ENTALIAS_INPUTRECTTOP,	
	ENTALIAS_INPUTBACK,
	ENTALIAS_INPUTSTRING,
	ENTALIAS_INPUTCURSOR,
	ENTALIAS_TOPBAR,
	ENTALIAS_HEADER,
	ENTALIAS_OUTPUTRECT,
	ENTALIAS_TIMER1,
	ENTALIAS_TIMER2,
	ENTALIAS_TIMER3,
	ENTALIAS_TIMER4,
	ENTALIAS_TIMER5,
	ENTALIAS_GEN1,
	ENTALIAS_GEN2,
	ENTALIAS_SCOREBOARDBACK,	
	ENTALIAS_SCORELINE1,	
	ENTALIAS_SCORELINE2,	
	ENTALIAS_SCORELINE3,	
	ENTALIAS_SCORELINE4,	
	ENTALIAS_SCORELINE5,	
	ENTALIAS_SCORELINE6,	
	ENTALIAS_GEN3,
	ENTALIAS_GEN4,
	ENTALIAS_GEN5,
	ENTALIAS_GEN6,
	ENTALIAS_GEN7,
	ENTALIAS_PGUP,
	ENTALIAS_PGDOWN,
	ENTALIAS_TAB,
	ENTALIAS_WPMRECT,
	ENTALIAS_WPMLBL,
	ENTALIAS_ACCRECT,
	ENTALIAS_ACCLBL,
	ENTALIAS_FUNRECT,
	ENTALIAS_FUNLBL,
	ENTALIAS_SCOREBACK,
	ENTALIAS_WPMSCORE,
	ENTALIAS_ACCSCORE,
	ENTALIAS_FUNSCORE,
	ENTALIAS_WPMBARBASE,
	ENTALIAS_ACCBARBASE,
	ENTALIAS_FUNBARBASE,
	ENTALIAS_WPMBAR,
	ENTALIAS_WPMBAREND,
	ENTALIAS_ACCBAR,
	ENTALIAS_ACCBAREND,
	
	ENTALIAS_DYNAMICSTART,
};

enum character_state {
	CHARSTATE_NEUTRAL,
	CHARSTATE_RIGHT,
	CHARSTATE_WRONG,
	CHARSTATE_BLACK,
	CHARSTATE_INPUT,
	CHARSTATE_UI,
};

struct entity_character {
	char glyph;
	enum character_state state;
};

struct entity_string {
 	struct entity_character contents[128];
	uint32_t length;
	uint32_t lengthInPixels;
	int32_t position;
};

struct entity_rect {
	union vec4 colour;
	uint32_t parentIndex;
};

enum entity_state {
	ENTSTATE_INVISIBLE = 0,	
	ENTSTATE_ISCLIPPED = 8,	
	ENTSTATE_DRAWONCE = 16,	
	ENTSTATE_WASDRAWN = 24,	
};

struct entity {
	enum entity_type type;
	uint64_t state;
	uint32_t index;
	union vec2 pos;
	union vec2 dim;
	struct rect2 clipRect;
	union {
		struct entity_string string;
		struct entity_rect rect;
	};
};

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
struct entity_event *NewEvent(struct game_state *, int32_t, enum event_types);

#endif