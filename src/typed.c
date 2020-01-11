/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Main implmentation:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*Main game loop plus everything 
 \/    \__, | .__/ \___|\__,_|		without a proper home
       |___/|_|               		
********************************************************************************/

static void ResetTimerRects(
	struct game_state *state)
{
	DeleteAllEventsByType(state, EVENT_FADE);
	for(int32_t i = ENTALIAS_TIMER1; i <= ENTALIAS_TIMER5; i++) {
		struct entity *timer = GetEntityByAlias(state, i);
		BITCLEAR(timer->state, ENTSTATE_INVISIBLE);
	}
}

static void InitTimerRects(
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

static void ReInitOutput(
	struct game_state *state)
{
	RePositionOutput(state);
	state->atLine = 0;
}

static void UpdateTime(
	struct game_timer *timer)
{
	timer->currentTime = (api.GetTime() - timer->baseTime);
}


static void DrawBackRect(
	struct game_state *state,
	struct entity *current)
{
	if(BITCHECK(current->state, ENTSTATE_DRAWONCE) &&
	   BITCHECK(current->state, ENTSTATE_WASDRAWN)) {
		return;
	} else {
		PushBackRect(state, current->pos, current->dim, current->rect.colour);
		if(BITCHECK(current->state, ENTSTATE_DRAWONCE)) {
			BITSET(current->state, ENTSTATE_WASDRAWN);
		}
	}
}

static void DrawRect(
	struct game_state *state,
	struct entity *current)
{
	if(BITCHECK(current->state, ENTSTATE_DRAWONCE) &&
	   BITCHECK(current->state, ENTSTATE_WASDRAWN)) {
		return;
	} else {
		union vec4 colour = current->rect.colour;
		if(BITCHECK(current->state, ENTSTATE_INVISIBLE)) {
			colour = FloatToVec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		PushRect(state, current->rect.assetIndex, current->pos, current->dim, colour);
		if(BITCHECK(current->state, ENTSTATE_DRAWONCE)) {
			BITSET(current->state, ENTSTATE_WASDRAWN);
		}
	}
}

static void DrawString(
	struct game_state *state,
	struct entity *current)
{
	if(BITCHECK(current->state, ENTSTATE_DRAWONCE) &&
	   BITCHECK(current->state, ENTSTATE_WASDRAWN)) {
		return;
	} else {
		struct rect2 cliprect = BITCHECK(current->state, ENTSTATE_ISCLIPPED) ?
			current->clipRect : FloatToRect2(0, 0, bufferX, bufferY);
		PushString(state, current, cliprect);
		if(BITCHECK(current->state, ENTSTATE_DRAWONCE)) {
			BITSET(current->state, ENTSTATE_WASDRAWN);
		}
	}
}

static void ProcessUpdateString(
	struct game_state *state,
	struct entity *current)
{
	if(ACTIVE_GAME(state) && !HasActiveEvent(state, current->index)) {
		switch(current->string.position) {
		case UPHASE_IDLE: {
			struct entity_event *event = NewEvent(state, current->index, EVENT_MOVELEFT);
			NewMoveEvent(state, event, FloatToVec2(-100, current->pos.y), FloatToVec2(-7.3f, 0));
			current->string.position = UPHASE_SCROLLOUT;
			break;
		} case UPHASE_SCROLLOUT: {
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
			break;
		} case UPHASE_SCROLLIN: {
			struct entity_event *event = NewEvent(state, current->index, EVENT_FLASH);
			NewFlashEvent(&state->timer, event, SECOND * 2, 150000ull);
			current->string.position = UPHASE_IDLE;
			break;
		} default: INVALID_PATH;
		}
	}
}


static void PrepAccString(
	struct game_state *state,
	struct entity *current)
{
	struct entity *accLbl = GetEntityByAlias(state, ENTALIAS_ACCSCORE);
	BITCLEAR(accLbl->state, ENTSTATE_WASDRAWN);
	BITCLEAR(state->entities[ENTALIAS_SCOREBACK].state, ENTSTATE_WASDRAWN);
	char buffer[12];
	sprintf(buffer, "%.0f%%", state->score.accuracey);
	CreateLabel(current, buffer, CHARSTATE_UI);
}

static void PrepWpmString(
	struct game_state *state,
	struct entity *current)
{
	struct entity *wpmLbl = GetEntityByAlias(state, ENTALIAS_WPMSCORE);
	BITCLEAR(wpmLbl->state, ENTSTATE_WASDRAWN);
	BITCLEAR(state->entities[ENTALIAS_SCOREBACK].state, ENTSTATE_WASDRAWN);
	char buffer[12];
	sprintf(buffer, "%d", state->score.wpm);
	CreateLabel(current, buffer, CHARSTATE_UI);
}

static bool DrawOutput(
	struct game_state *state,
	struct entity *current)
{
	return(current->string.position < state->atLine - 1 || 
	       current->string.position > state->atLine + 5);
}

static void IncrementProgressRect(
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

static void UpdateProgressRect(
	struct game_state *state,
	struct entity *current)
{
	if(RETURN_ISSET(state)) {
		current->dim.x = 0;
		current->pos.y -= 25.0f;
		struct entity_event *event = NewEvent(state, current->index, EVENT_MOVEUP);
		NewMoveEvent(state, event, FloatToVec2(current->pos.x, current->pos.y + 25.0f),
			FloatToVec2(0, 6.0f));
	} else if(INPUT_ISSET(state)) {
		IncrementProgressRect(state, current);
	}
}

static void ProcessInputString(
	struct game_state *state,
	struct entity *current)
{
	if(RETURN_ISSET(state)) {
		current->string.length = 0;
		current->string.lengthInPixels = 0;
	} else if(INPUT_ISSET(state)) {		
		current->string.contents[current->string.length++].glyph =
			state->input.inputCharacter;
		current->string.lengthInPixels += GetCharacterWidth(state,
			state->input.inputCharacter);
	}
}

static void ProcessEntities(
	struct game_state *state)
{
	for(int32_t i = 0; i < MAX_ENTITIES; i++) {
		struct entity *current = &state->entities[i];
		switch(current->type) {
		case ENTTYPE_SCORELABEL:
		case ENTTYPE_LABELSTRING: {
			DrawString(state, current);
			break;
		} case ENTTYPE_INPUTSTRING: {
			ProcessInputString(state, current);
			DrawString(state, current);
			break;
		} case ENTTYPE_OUTPUTSTRING: {
			ProcessOutputString(state, current);
			if(!DrawOutput(state, current)) {
				DrawString(state, current);
			}			
			break;
		} case ENTTYPE_UPDATESTRING: {
			ProcessUpdateString(state, current);
			DrawString(state, current);
			break;
		} case ENTTYPE_CURSORRECT: {
			struct entity *inputControl = &state->entities[current->rect.parentIndex];
			assert(inputControl);
			union vec4 colour = FloatToVec4(0.0f, 0.0f, 0.0f, 1.0f);
			if(!BITCHECK(current->state, ENTSTATE_INVISIBLE)) {
				colour = current->rect.colour;
			}
			union vec2 newPos = {
				.x = current->pos.x + inputControl->string.lengthInPixels,
				.y = current->pos.y
			};
			PushRect(state, current->rect.assetIndex, newPos, current->dim, colour);
			break;
		} case ENTTYPE_GENERICRECT: {
			if(current->dim.x < 1) break;
			DrawRect(state, current);
			break;
		} case ENTTYPE_PROGRESSRECT: {
			UpdateProgressRect(state, current);			
			DrawBackRect(state, current);
			break;
		} case ENTTYPE_WPMSTRING: {
			if(ACTIVE_GAME(state)) {
				PrepWpmString(state, current);
			}
			DrawString(state, current);
			break;
		} case ENTTYPE_ACCSTRING: {
			if(ACTIVE_GAME(state)) {
				PrepAccString(state, current);
			}
			DrawString(state, current);
			break;
		} case ENTTYPE_BACKRECT: {
			DrawBackRect(state, current);
			break;
		} default: break;
		}
	}
}

static void UpdateAcc(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	struct entity *accBar = GetEntityByAlias(state, ENTALIAS_ACCBAR);
	accBar->dim.x = (float)score->accuracey * 2.5f;
	struct entity *accBarEnd = GetEntityByAlias(state, ENTALIAS_ACCBAREND);
	accBarEnd->pos.x = accBar->dim.x + accBar->pos.x;
}

static void UpdateWPM(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	struct entity *wpmBar = GetEntityByAlias(state, ENTALIAS_WPMBAR);
	wpmBar->dim.x = (float)score->wpm > 100.0f ? 100.0f * 2.5f : (float)score->wpm * 2.5f;
	struct entity *wpmBarEnd = GetEntityByAlias(state, ENTALIAS_WPMBAREND);
	wpmBarEnd->pos.x = wpmBar->dim.x + wpmBar->pos.x;
}

static void ClearInput(
	struct game_state *state)
{
	struct entity *inputControl = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	inputControl->string.length = 0;
	inputControl->string.lengthInPixels = 0;
}

extern void RestartGame(
	struct game_state *state)
{
	api.DeAllocate(state->outputStringBuffer);
	
	ResetTimerRects(state);
	srand(state->timer.currentTime);
	BITCLEAR(state->global, GLOBAL_POST);
	BITCLEAR(state->global, GLOBAL_GAME);
	
	state->outputStringBufferSize = 0;
	struct game_score nullScore = {};
	state->score = nullScore;
	state->atLine = 0;
	GetEntityByAlias(state, ENTALIAS_WPMBAR)->dim.x = 0;
	GetEntityByAlias(state, ENTALIAS_ACCBAR)->dim.x = 0;
	UpdateWPM(state);
	UpdateAcc(state);
	RedrawAll(state);
	DeleteAllEntitiesOfType(state, ENTTYPE_OUTPUTSTRING);	
	ClearInput(state);
	CreateOutputEntities(state, bufferX, bufferY);	
}

extern void NewGame(
	struct game_state *state)
{
	InitTimerRects(state);	

	state->timer.gameStartTime = state->timer.currentTime;
	state->timer.gameLength = SECOND * 5;
	state->atLine = 0;

	ReInitOutput(state);
	BITSET(state->global, GLOBAL_GAME);
}

static void EndGame(
	struct game_state *state)
{
	BITCLEAR(state->global, GLOBAL_GAME);
	BITSET(state->global, GLOBAL_POST);
	struct entity *progressRect = GetProgressRect(state);
	DeleteEntityAt(state, progressRect->index);
	struct entity *inputString = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	inputString->string.length = 0;
	inputString->string.lengthInPixels = 0;
	ResetTimerRects(state);
	RedrawAll(state);
	NewScore(state);
}

static void InitState(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	bufferX = buffer->x;
	bufferY = buffer->y;
	state->timer.baseTime = api.GetTime();
	srand(state->timer.baseTime);
	CreateUi(state, buffer);
	CreateOutputEntities(state, buffer->x, buffer->y);
	CreateLabel(GetEntityByAlias(state, ENTALIAS_HEADER),
		headerStrings[rand() % ARRAY_COUNT(headerStrings)], CHARSTATE_UI);
	BITSET(state->global, GLOBAL_INIT);
}

static bool TimeIsUp(
	struct game_timer *timer)
{
	return(timer->currentTime - timer->gameStartTime > 
	       timer->gameLength);
}

static void EndFrame(
	struct game_state *state)
{
	state->input.inputCharacter = '\0';
	state->input.state = 0;		
}

extern void GameLoop(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	if(!BITCHECK(state->global, GLOBAL_INIT)) {
		InitState(state, buffer);
	}
	UpdateTime(&state->timer);
	ReceiveInput(state, GetEntityByAlias(state, ENTALIAS_INPUTSTRING));	
	if(ACTIVE_GAME(state)) {
		if(TimeIsUp(&state->timer)) {
			EndGame(state);
		} else if(ComputeScore(state)) {			
			RedrawAll(state);	
		}
		UpdateWPM(state);
		UpdateAcc(state);
	}	
	ProcessEntities(state);	
	ProcessEvent(state);
	ProcessRenderJobs(state, buffer);
	EndFrame(state);
}