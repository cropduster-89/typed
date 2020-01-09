/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Input implmentation
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		TODO Move scoring & entity logic out somewhow
 \/    \__, | .__/ \___|\__,_|		double up input state to received->confirmed
       |___/|_|               		and processed in game code?
********************************************************************************/

static void IncrementProgressRect(
	struct game_state *state,
	struct entity *inputControl)
{
	struct entity *progressRect = GetProgressRect(state);
	struct entity *line = GetCurrentOutputLine(state);
	char character = line->string.contents[inputControl->string.length - 1].glyph;
	if(character == '\0') {return;}
	else {
		progressRect->dim.x += GetCharacterWidth(state, character);
	} 
}

extern bool InputIsCorrect(
	struct game_state *state,
	struct entity *inputString)
{
	struct entity *line = GetCurrentOutputLine(state);	
	bool result = state->input.inputCharacter == 
		line->string.contents[inputString->string.length].glyph;
	return(result);
}

static void TakeInput(
	struct game_state *state,
	struct entity *inputControl)
{	
	state->score.lettersTyped++;
	if(!InputIsCorrect(state, inputControl)) {
		state->score.lettersWrong++;
	}
}

#define RETURN_ISSET(state) BITCHECK(state->input.state, ISTATE_RETURN)

static bool TakingPgUp(
	struct game_state *state)
{
	return(!BITCHECK(state->global, GLOBAL_GAME) && BITCHECK(state->input.state, ISTATE_PGUP) &&
		state->atLine > 0 && FreeEvent(state));
}

static bool TakingPgDn(
	struct game_state *state)
{
	return(!BITCHECK(state->global, GLOBAL_GAME) && BITCHECK(state->input.state, ISTATE_PGDOWN) &&
		state->atLine < state->outputLines && FreeEvent(state));
}

static bool TakingInput(
	struct game_state *state,
	struct entity *inputControl)
{
	return(state->input.inputCharacter != '\0' && !BITCHECK(state->global, GLOBAL_POST) &&
	       inputControl->string.lengthInPixels < inputControl->dim.x);
}

extern bool EndOfLine(
	struct game_state *state,
	struct entity *inputString)
{
	struct entity *line = GetCurrentOutputLine(state);		
	return(inputString->string.length >= line->string.length);
}

static bool TakingReturn(
	struct game_state *state,
	struct entity *inputControl)
{
	return(BITCHECK(state->global, GLOBAL_GAME) &&
	       RETURN_ISSET(state) &&
	       state->atLine < state->outputLines && FreeEvent(state) &&
	       EndOfLine(state, inputControl));
}

static bool ReceiveInput(
	struct game_state *state,
	struct entity *inputString)
{
	bool result = false;
	if(TakingInput(state, inputString)) {
		result = true;
		if(!BITCHECK(state->global, GLOBAL_GAME)) {
			NewGame(state);
		}
		TakeInput(state, inputString);
	} 
	
	if(TakingReturn(state, inputString)) {
		state->atLine++;
		//TakeReturn(state, inputString);
		ScrollOutput(state, true);
	} else {
		BITCLEAR(state->input.state, ISTATE_RETURN);
	}
	
	if(TakingPgUp(state)) {
		state->atLine--;
		ScrollOutput(state, false);
	} else if(TakingPgDn(state)) {
		state->atLine++;
		ScrollOutput(state, true);
	} else if(BITCHECK(state->input.state, ISTATE_TAB)) {
		RestartGame(state);
	}
	return(result);
}