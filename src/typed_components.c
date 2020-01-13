/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Process componenets:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*everything without a proper home
 \/    \__, | .__/ \___|\__,_|		can live here <3
       |___/|_|               		
********************************************************************************/

extern void ResetTimerRects(
	struct game_state *state)
{
	DeleteAllEventsByType(state, EVENT_FADE);
	for(int32_t i = ENTALIAS_TIMER1; i <= ENTALIAS_TIMER5; i++) {
		struct entity *timer = GetEntityByAlias(state, i);
		BITCLEAR(timer->state, ENTSTATE_INVISIBLE);
	}
}

extern void InitTimerRects(
	struct game_state *state)
{
	int32_t timers = ENTALIAS_TIMER5 - ENTALIAS_TIMER1 + 1;
	for(int32_t i = ENTALIAS_TIMER1; i <= ENTALIAS_TIMER5; i++) {
		int32_t nI = abs(ENTALIAS_TIMER1 - i);
		int32_t timerI = timers - nI;

		struct entity *timer = GetEntityByAlias(state, i);
		BITCLEAR(timer->state, ENTSTATE_INVISIBLE);
		struct entity_event *event = NewEvent(state, timer->index, EVENT_FADE);
		NewFadeEvent(&state->timer, event, SECOND * (timerI * 10));
	}
}

extern void ReInitOutput(
	struct game_state *state)
{
	RePositionOutput(state);
	state->atLine = 0;
}

extern void IncrementProgressRect(
	struct game_state *state,
	struct entity *current)
{
	struct entity *progressRect = GetProgressRect(state);
	struct entity *line = GetCurrentOutputLine(state);
	struct entity *inputString = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	char character = line->string.contents[inputString->string.length - 1].glyph;
	if(character == '\0') {return;}
	else {
		progressRect->dim.x += GetCharacterWidth(state, character);
	} 
}

extern void UpdateProgressRect(
	struct game_state *state,
	struct entity *current)
{
	if(RETURN_ISSET(state)) {
		current->dim.x = 0;
		current->pos.y -= 25.0f;
		struct entity_event *event = NewEvent(state, current->index, EVENT_MOVEUP);
		NewMoveEvent(state, event, FloatToVec2(current->pos.x, current->pos.y + 25.0f),
			FloatToVec2(0, 6.0f));
	} else if(INPUT_ISSET(state) && !INPUT_WRONG_ISSET(state)) {
		IncrementProgressRect(state, current);
	}
}

extern void ProcessInputString(
	struct game_state *state,
	struct entity *current)
{
	if(RETURN_ISSET(state)) {
		current->string.length = 0;
		current->string.lengthInPixels = 0;
		struct entity *cursorRect = GetEntityByAlias(state, ENTALIAS_INPUTCURSOR);
		cursorRect->pos.x = current->pos.x;	
	} else if(INPUT_ISSET(state)) {		
		current->string.contents[current->string.length++].glyph =
			state->input.inputCharacter;
		uint32_t newLength = GetCharacterWidth(state,
			state->input.inputCharacter);
		current->string.lengthInPixels += newLength;
		struct entity *cursorRect = GetEntityByAlias(state, ENTALIAS_INPUTCURSOR);
		cursorRect->pos.x += newLength;	
	}
}

extern void CreateProgressRect(
	struct game_state *state)
{
	union vec2 progressBarPos = {
		.x = bufferX * 0.25f,
		.y = bufferY - 80.0f - 33.0f
	};
	union vec2 ProgressBarDim = {
		.x = 0,
		.y = 2.0f
	};
	struct entity *bannerRect = NewEntity(state, progressBarPos, ProgressBarDim, ENTTYPE_PROGRESSRECT);
	bannerRect->rect.colour = COL_GREEN;
}

extern void UpdateAcc(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	struct entity *accBar = GetEntityByAlias(state, ENTALIAS_ACCBAR);
	accBar->dim.x = (float)score->accuracey * 2.5f;
}

extern void UpdateWPM(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	struct entity *wpmBar = GetEntityByAlias(state, ENTALIAS_WPMBAR);
	wpmBar->dim.x = (float)score->wpm > 100.0f ? 100.0f * 2.5f : (float)score->wpm * 2.5f;
}

extern void ClearInput(
	struct game_state *state)
{
	struct entity *inputControl = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	inputControl->string.length = 0;
	inputControl->string.lengthInPixels = 0;
}

static void UpdateInitScrollOut(
	struct game_state *state,
	struct entity *current)
{
	struct entity_event *event = NewEvent(state, current->index, EVENT_MOVELEFT);
	NewMoveEvent(state, event, FloatToVec2(-100, current->pos.y), FloatToVec2(-7.3f, 0));
	current->string.position = UPHASE_SCROLLOUT;
}

static void UpdateInitScrollIn(
	struct game_state *state,
	struct entity *current)
{
	struct entity_event *event = NewEvent(state, current->index, EVENT_MOVELEFT);
	current->pos.x = bufferX;
	union vec2 destination = {
		.x = bufferX * 0.25f + 12.0f,
		.y = current->pos.y
	};
	NewMoveEvent(state, event, destination, FloatToVec2(-7.3f, 0));
	char *newUpdate;
	if(state->score.wpm < 60) {
		newUpdate = encouragementStrings[rand() % ARRAY_COUNT(encouragementStrings)];
	} else {
		newUpdate = praiseStrings[rand() % ARRAY_COUNT(praiseStrings)];
	}
	CreateLabel(current, newUpdate, CHARSTATE_BLACK);
	current->string.position = UPHASE_SCROLLIN;
}

static void UpdateInitIdle(
	struct game_state *state,
	struct entity *current)
{
	struct entity_event *event = NewEvent(state, current->index, EVENT_FLASH);
	NewFlashEvent(&state->timer, event, SECOND * 2, 150000ull);
	current->string.position = UPHASE_IDLE;
}

extern void ProcessUpdateString(
	struct game_state *state,
	struct entity *current)
{
	if(ACTIVE_GAME(state) && !HasActiveEvent(state, current->index)) {
		switch(current->string.position) {
		case UPHASE_IDLE: {
			UpdateInitScrollOut(state, current);
			break;
		} case UPHASE_SCROLLOUT: {
			UpdateInitScrollIn(state, current);
			break;
		} case UPHASE_SCROLLIN: {
			UpdateInitIdle(state, current);
			break;
		} default: INVALID_PATH;
		}
	}
}