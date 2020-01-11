#pragma once
#ifndef TYPED_H
#define TYPED_H

/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		*Main header 
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |
  \/    \__, | .__/ \___|\__,_|
        |___/|_|               
********************************************************************************/

#define MAX_ENTITIES 128
#define MAX_EVENTS 24
#define MAX_RENDERJOBS 1024

#define ACTIVE_GAME(state) BITCHECK(state->global, GLOBAL_GAME)

struct screen_buffer {
	int32_t x;
	int32_t y;
	int32_t stride;

	void *data;
};

struct game_timer {
	uint64_t baseTime;
	uint64_t currentTime;
	uint64_t gameStartTime;
	uint64_t gameLength;
};

enum global_game_state {
	GLOBAL_INIT = 0,
	GLOBAL_POST = 8,
	GLOBAL_GAME = 16,
};

struct game_state {
	struct input_control input;	
	struct game_timer timer;
	struct game_score score;
	
	void *characterBuffer;
	void *bmpBuffer;
	
	char *outputStringBuffer;
	uint32_t outputStringBufferSize;	
		
	union render_job renderJobs[MAX_RENDERJOBS];
	uint32_t renderJobCount;	
	struct entity entities[MAX_ENTITIES];
	uint32_t entityCount;	
	struct entity_event events[MAX_EVENTS];
	uint32_t eventCount;	
	
	int32_t atLine;
	uint32_t outputLines;
	
	uint32_t global;
};

struct entity *GetCurrentOutputLine(struct game_state *);
void NewGame(struct game_state *);
void RestartGame(struct game_state *);

#endif