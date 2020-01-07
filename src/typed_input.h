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

enum input_state {
	ISTATE_RETURN = 0,
	ISTATE_PGUP = 8,
	ISTATE_PGDOWN = 16,
	ISTATE_TAB = 24
};

union button_state {
	bool endedDown;
	bool isDown;
};

struct input_control {
	char inputCharacter;
	uint32_t state;
	
	union button_state shift;
};

#endif