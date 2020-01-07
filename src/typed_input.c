static void IncrementProgressRect(
	struct game_state *state,
	struct entity *inputControl)
{
	struct entity *progressRect = GetProgressRect(state);
	struct entity *line = GetCurrentOutputLine(state);
	char character = line->string.contents[inputControl->string.length - 1].glyph;
	if(character == '\0') {return;}
	else if(character != ' ') {
		progressRect->dim.x += GetCharacterWidth(state, character);
	} else {
		progressRect->dim.x += 10.0f;
	}
}

static void DeIncrementProgressRect(
	struct game_state *state,
	struct entity *inputControl)
{
	struct entity *progressRect = GetProgressRect(state);
	struct entity *line = GetCurrentOutputLine(state);
	char character = line->string.contents[inputControl->string.length - 1].glyph;
	if(character == '\0') {return;}
	else if(character != ' ') {
		progressRect->dim.x -= GetCharacterWidth(state, character);
	} else {
		progressRect->dim.x -= 10.0f;
	}
}

static void TakeInput(
	struct game_state *state,
	struct entity *inputControl)
{
	inputControl->string.contents[inputControl->string.length++].glyph =
		state->input.inputCharacter;
	if(state->input.inputCharacter != ' ') {
		inputControl->string.lengthInPixels += GetCharacterWidth(state,
			state->input.inputCharacter);
	} else {
		inputControl->string.lengthInPixels += 10.0f;
	}
	IncrementProgressRect(state, inputControl);
}

static void TakeBackspace(
	struct game_state *state,
	struct entity *inputControl)
{
	DeIncrementProgressRect(state, inputControl);
	char removedChar = inputControl->string.
		contents[inputControl->string.length - 1].glyph;
	inputControl->string.length--;
	inputControl->string.lengthInPixels -= GetCharacterWidth(state, removedChar);
	BITCLEAR(state->input.state, ISTATE_BACKSPACE);
}

extern void ClearInput(
	struct game_state *state)
{
	struct entity *inputControl = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	inputControl->string.length = 0;
	inputControl->string.lengthInPixels = 0;
}

static void TakeReturn(
	struct game_state *state,
	struct entity *inputControl)
{
	struct game_score *score = &state->score;
	score->correctWordsCarry += score->correctWords;
	score->lettersWrongCarry += score->lettersWrong;
	score->lettersTypedCarry += score->lettersTyped;
	struct entity *progressRect = GetProgressRect(state);
	progressRect->dim.x = 0;
	progressRect->pos.y -= 25.0f;
	struct entity_event *event = NewEvent(state, progressRect->index, EVENT_MOVEUP);
		NewMoveEvent(state, event, FloatToVec2(progressRect->pos.x, progressRect->pos.y + 25.0f),
			FloatToVec2(0, 6.0f));
	inputControl->string.length = 0;
	inputControl->string.lengthInPixels = 0;
}

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

static bool TakingReturn(
	struct game_state *state,
	struct entity *inputControl)
{
	return(BITCHECK(state->global, GLOBAL_GAME) &&
	       BITCHECK(state->input.state, ISTATE_RETURN) &&
		state->atLine < state->outputLines && FreeEvent(state));
}

static bool TakingBackspace(
	struct game_state *state,
	struct entity *inputControl)
{
	return(BITCHECK(state->input.state, ISTATE_BACKSPACE) &&
		inputControl->string.length > 0);
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
	} else if(TakingBackspace(state, inputString)) {
		TakeBackspace(state, inputString);
		result = true;
	} else if(TakingReturn(state, inputString)) {
		state->atLine++;
		TakeReturn(state, inputString);
		ScrollOutput(state, true);
	} else if(TakingPgUp(state)) {
		state->atLine--;
		ScrollOutput(state, false);
	} else if(TakingPgDn(state)) {
		state->atLine++;
		ScrollOutput(state, true);
	} else if(BITCHECK(state->input.state, ISTATE_TAB)) {
		RestartGame(state);
	}
	state->input.state = 0;
	state->input.inputCharacter = '\0';
	return(result);
}

extern void InputControl(
	struct game_state *state,
	struct entity *inputControl)
{
	if(ReceiveInput(state, inputControl)) {
		CompareInput(state, inputControl);
	}
}