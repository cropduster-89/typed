#pragma once
#ifndef TYPED_STRING_H
#define TYPED_STRING_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

enum string_slots {
	SLOT_NOUN,
	SLOT_VERBING,
	SLOT_VERB,
	SLOT_ADJ,
	SLOT_ADJLY,
};

enum character_state {
	CHARSTATE_NEUTRAL,
	CHARSTATE_RIGHT,
	CHARSTATE_WRONG,
	CHARSTATE_BLACK,
	CHARSTATE_INPUT,
	CHARSTATE_UI,
};

uint32_t GetCharacterWidth(struct game_state *, char glyph);
void ChangeLabelState(struct entity *, enum character_state);

#endif