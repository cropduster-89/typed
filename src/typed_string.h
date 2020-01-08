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

uint32_t GetCharacterWidth(struct game_state *, char glyph);

#endif