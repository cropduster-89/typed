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

#define MAX_STRINGLENGTH 64

/*
*	Entity types, strings first,
*	rects second
*/
enum entity_type {
	ENTTYPE_NULL,	
	
	ENTTYPE_UPDATESTRING,
	ENTTYPE_LABELSTRING,
	ENTTYPE_WPMSTRING,
	ENTTYPE_ACCSTRING,
	ENTTYPE_INPUTSTRING,
	ENTTYPE_OUTPUTSTRING,
	ENTTYPE_SCORELABEL,
	ENTTYPE_CURSORRECT,
	ENTTYPE_GENERICRECT,
	ENTTYPE_PROGRESSRECT,
	ENTTYPE_BANNERRECT,
	ENTTYPE_OUTPUTCLIPRECT,	
	
	ENTTYPE_COUNT
};

#define STRING_START ENTTYPE_UPDATESTRING
#define RECT_START ENTTYPE_CURSORRECT

/*
*	All entities that are created at startup
*	have a hardcoded index alias, all those
*	that are created dynamincally start
* 	from ENTALIAS_DYNAMICSTART.
*
*	TODO create static entities by index so 
*	creation order stops beng important
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

struct entity_character {
	char glyph;
	enum character_state state;
};

struct entity_string {
 	struct entity_character contents[MAX_STRINGLENGTH];
	uint32_t length;
	uint32_t lengthInPixels;		
	int32_t backgroundIndex;
	int32_t backgroundCount;
	union {
		int32_t position;
		int32_t score;
	};
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

/*
*	TODO: Strings are much bigger than rects,
*	move them out to fullon ent/comp 
*	to save space
*/

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

#endif