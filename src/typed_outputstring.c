/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		Output String implementation:
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |		*handle state and events for 
  \/    \__, | .__/ \___|\__,_|		lines of output text
        |___/|_|               
********************************************************************************/

static void ScrollOutput(
	struct game_state *state,
	struct entity *current,
	bool isDown)
{		
	if(isDown) {
		struct entity_event *event = NewEvent(state, current->index, EVENT_MOVEUP);
		NewMoveEvent(state, event, FloatToVec2(current->pos.x, current->pos.y + 25.0f),
			FloatToVec2(0, 6.0f));			
	} else {
		struct entity_event *event = NewEvent(state, current->index, EVENT_MOVEDOWN);
		NewMoveEvent(state, event, FloatToVec2(current->pos.x, current->pos.y - 25.0f),
			FloatToVec2(0, -6.0f));
	}	
}

static bool IsActiveOutputLine(
	struct game_state *state,
	struct entity *current)
{
	return((INPUT_ISSET(state) || INPUT_WRONG_ISSET(state)) && current == GetCurrentOutputLine(state));
}

#define GetCorrectChar(state, current)  _GetCharFromOutputLine(state, current, 1)
#define GetIncorrectChar(state, current)  _GetCharFromOutputLine(state, current, 0)
static struct entity_character *_GetCharFromOutputLine(
	struct game_state *state,
	struct entity *current,
	int32_t offset)
{
	uint32_t length = GetEntityByAlias(state, ENTALIAS_INPUTSTRING)->string.length - offset;
#ifdef DEBUG
	assert(length >= 0 && length < MAX_STRINGLENGTH);
#endif
	struct entity_character *letter = &current->string.contents[length];	
#ifdef DEBUG
	assert(letter);
#endif
	return(letter);
}

static void ChangeLetterState(
	struct game_state *state,
	struct entity *current)
{
	if(INPUT_ISSET(state)) {
		struct entity_character *letter = GetCorrectChar(state, current);				
		if(letter->state == CHARSTATE_NEUTRAL) {
			letter->state = CHARSTATE_RIGHT;
		}			
	} else if(INPUT_WRONG_ISSET(state) && state->input.inputCharacter != '\0') {
		struct entity_character *letter = GetIncorrectChar(state, current);
		letter->state = CHARSTATE_WRONG;
	}	
}

extern void ProcessOutputString(
	struct game_state *state,
	struct entity *current)
{	
	if(IsActiveOutputLine(state, current)) {
		ChangeLetterState(state, current);
	}

	if(PGDOWN_ISSET(state) || RETURN_ISSET(state)) {			
		ScrollOutput(state, current, true);
	} else if(PGUP_ISSET(state)) {		
		ScrollOutput(state, current, false);
	}
}