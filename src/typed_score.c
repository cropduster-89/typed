/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		Score implementation
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |		*alomst decoupled, except for CompareInput, change letter state elsewhere? 
  \/    \__, | .__/ \___|\__,_|		*TODO no need for ComputeCorrectWords
        |___/|_|               		to loop anymore, fix
********************************************************************************/

static void ComputeCorrectWords(
	struct game_state *state,
	struct entity *inputString)
{
	struct entity *line = GetCurrentOutputLine(state);
	bool completeWord = true;
	uint32_t correctWords = 0;	
	for(int32_t i = 0; i < inputString->string.length; ++i) {
		char input = inputString->string.contents[i].glyph;
		char output = line->string.contents[i].glyph;
		bool correctLetter = (input == output);
		if(input == ' ' || line->string.contents[i + 1].glyph == '\0') {
			if(completeWord) {
				correctWords++;
			}
			completeWord = true;
		}
		if(!correctLetter) {
			completeWord = false;
		}
	}
	state->score.correctWords = correctWords;
}

static bool InputIsCorrect(
	struct game_state *state)
{	
	uint32_t length = GetEntityByAlias(state, ENTALIAS_INPUTSTRING)->string.length;	
	char output = GetCurrentOutputLine(state)->string.contents[length].glyph;	
	
	return(state->input.inputCharacter == output);
}

static void TakeInput(
	struct game_state *state,
	struct entity *inputControl)
{	
	state->score.lettersTyped++;
	if(!InputIsCorrect(state)) {
		state->score.lettersWrong++;
		INPUT_CLEAR(state);
		INPUT_WRONG_SET(state);
	}
}

static uint32_t GetWpm(
	struct game_timer *timer,
	struct game_score *score)
{
	float minsElapsed = (float)(timer->currentTime - timer->gameStartTime) / 60000000.0f;
	uint32_t totalCorrect = score->correctWords + score->correctWordsCarry;
	uint32_t result = (float)totalCorrect / minsElapsed;
	
	return(result);
}

static float GetAcc(
	struct game_score *score)
{
	float value = score->lettersTyped - score->lettersWrong;
	float result = (float)value / (float)score->lettersTyped * 100.0f;
	
	return(result);
}

static void UpdateScore(
	struct game_state *state)
{
	struct game_score *score = &state->score;	
	if(RETURN_ISSET(state)) {
		score->correctWordsCarry += score->correctWords;
		score->correctWords = 0;
	}
	score->wpm = GetWpm(&state->timer, score);
	score->accuracey = GetAcc(score);	
}

extern bool ComputeScore(
	struct game_state *state)
{
	bool result = false; 
	if(state->input.inputCharacter != '\0') {
		struct entity *inputString = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
		TakeInput(state, inputString);
		ComputeCorrectWords(state, inputString);
		result = true;	
	}
	UpdateScore(state);	
	return(result);
}