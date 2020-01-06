#pragma once
#ifndef TYPED_INPUT_H
#define TYPED_INPUT_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

#include <stdint.h>

enum input_state {
	ISTATE_BACKSPACE = 0,
	ISTATE_RETURN = 8,
	ISTATE_PGUP = 16,
	ISTATE_PGDOWN = 24,
	ISTATE_TAB = 32
};

union button_state {
	bool endedDown;
	bool isDown;
};

struct input_control {
	char inputCharacter;
	uint64_t state;
	
	union button_state shift;
};

#endif