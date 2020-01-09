#pragma once
#ifndef TYPED_SCORE_H
#define TYPED_SCORE_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Score header
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

struct game_score {
	float accuracey;
	uint32_t lettersTyped;
	uint32_t lettersWrong;
	
	uint32_t wpm;
	uint32_t correctWords;
	uint32_t correctWordsCarry;
};

#endif