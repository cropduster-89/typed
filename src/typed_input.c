/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Input implmentation:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*Gates input from platform code to be sent 
 \/    \__, | .__/ \___|\__,_|		forward or cleared 
       |___/|_|               		
					*More or less done? 
********************************************************************************/

static bool TakingPgUp(
	struct game_state *state)
{
	return(!ACTIVE_GAME(state) && state->atLine > 0 && FreeEvent(state));
}

static bool TakingPgDn(
	struct game_state *state)
{
	return(!ACTIVE_GAME(state) && state->atLine < state->outputLines && FreeEvent(state));
}

static bool EndOfLine(
	struct game_state *state,
	struct entity *inputString)
{
	int32_t length = GetCurrentOutputLine(state)->string.length - 2;		
	return(inputString->string.length >= length);
}

static bool TakingInput(
	struct game_state *state,
	struct entity *inputControl)
{
	return(state->input.inputCharacter != '\0' && !BITCHECK(state->global, GLOBAL_POST) &&
	       inputControl->string.lengthInPixels < inputControl->dim.x && !EndOfLine(state, inputControl));
}

static bool TakingReturn(
	struct game_state *state,
	struct entity *inputControl)
{
	return(ACTIVE_GAME(state) &&	       
	       state->atLine < state->outputLines && FreeEvent(state) &&
	       EndOfLine(state, inputControl));
}

extern void ReceiveInput(
	struct game_state *state,
	struct entity *inputString)
{
	if(TakingInput(state, inputString)) {		
		if(!ACTIVE_GAME(state)) {
			NewGame(state);
		}
		INPUT_SET(state);
	}
	
	if(RETURN_ISSET(state) && TakingReturn(state, inputString)) {
		state->atLine++;		
	} else {
		RETURN_CLEAR(state);
	}
	
	if(PGUP_ISSET(state) && TakingPgUp(state)) {
		state->atLine--;	
	} else {
		PGUP_CLEAR(state);
	}
	
	if(PGDOWN_ISSET(state) && TakingPgDn(state)) {
		state->atLine++;	
	} else {
		PGDOWN_CLEAR(state);
	}
	
	if(TAB_ISSET(state)) {
		RestartGame(state);
	}
}