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
#define MAX_RENDERJOBS 1028

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
	
	void *characterBuffer;
	
	char *outputStringBuffer;
	uint32_t outputStringBufferSize;	
		
	union render_job renderJobs[MAX_RENDERJOBS];
	uint32_t renderJobCount;	
	struct entity entities[MAX_ENTITIES];
	uint32_t entityCount;	
	struct entity_event events[MAX_EVENTS];
	uint32_t eventCount;
	
	float accuracey;
	uint32_t lettersTyped;
	uint32_t lettersTypedCarry;
	uint32_t lettersWrong;
	uint32_t lettersWrongCarry;
	uint32_t wpm;
	uint32_t correctWords;
	uint32_t correctWordsCarry;
	
	int32_t atLine;
	uint32_t outputLines;
	
	uint32_t global;
};

struct entity *GetCurrentOutputLine(struct game_state *);
void NewGame(struct game_state *);
uint32_t GetWpm(struct game_timer *, uint32_t);
void CompareInput(struct game_state *, struct entity *);
void RestartGame(struct game_state *);

#endif