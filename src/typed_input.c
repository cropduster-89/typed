/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Input implmentation:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*Gates input to be sent forward or  
 \/    \__, | .__/ \___|\__,_|		cleared 
       |___/|_|               		
					*More or less done? still need to move 
					scrolling out
********************************************************************************/

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

static bool EndOfLine(
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

extern void ReceiveInput(
	struct game_state *state,
	struct entity *inputString)
{
	if(TakingInput(state, inputString)) {		
		if(!BITCHECK(state->global, GLOBAL_GAME)) {
			NewGame(state);
		}
		INPUT_SET(state);
	}
	
	if(TakingReturn(state, inputString)) {
		state->atLine++;
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
}