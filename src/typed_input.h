#pragma once
#ifndef TYPED_INPUT_H
#define TYPED_INPUT_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		*input header
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

#define RETURN_ISSET(state) BITCHECK(state->input.state, ISTATE_RETURN)
#define RETURN_SET(state) BITSET(state->input.state, ISTATE_RETURN)
#define RETURN_CLEAR(state) BITCLEAR(state->input.state, ISTATE_RETURN)

#define PGUP_ISSET(state) BITCHECK(state->input.state, ISTATE_PGUP)
#define PGUP_SET(state) BITSET(state->input.state, ISTATE_PGUP)
#define PGUP_CLEAR(state) BITCLEAR(state->input.state, ISTATE_PGUP)

#define PGDOWN_ISSET(state) BITCHECK(state->input.state, ISTATE_PGDOWN)
#define PGDOWN_SET(state) BITSET(state->input.state, ISTATE_PGDOWN)
#define PGDOWN_CLEAR(state) BITCLEAR(state->input.state, ISTATE_PGDOWN)

#define TAB_ISSET(state) BITCHECK(state->input.state, ISTATE_TAB)
#define TAB_SET(state) BITSET(state->input.state, ISTATE_TAB)
#define TAB_CLEAR(state) BITCLEAR(state->input.state, ISTATE_TAB)

#define INPUT_ISSET(state) BITCHECK(state->input.state, ISTATE_INPUT)
#define INPUT_SET(state) BITSET(state->input.state, ISTATE_INPUT)
#define INPUT_CLEAR(state) BITCLEAR(state->input.state, ISTATE_INPUT)

enum input_state {
	ISTATE_RETURN = 0,
	ISTATE_PGUP = 8,
	ISTATE_PGDOWN = 16,
	ISTATE_TAB = 24,
	ISTATE_INPUT = 32
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