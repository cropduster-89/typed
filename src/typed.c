static void RedrawOutput(
	struct game_state *state)
{
	BITCLEAR(state->entities[ENTALIAS_OUTPUTRECT].state, ENTSTATE_WASDRAWN);
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < state->entityCount; ++i) {
		if(state->entities[i].type == ENTTYPE_OUTPUTSTRING) {
			BITCLEAR(state->entities[i].state, ENTSTATE_WASDRAWN);
		}
	}
}

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

static void NewScore(
	struct game_state *state)
{
	union vec2 scoreBasePos = {
		.x = 20.0f,
		.y = (float)bufferY - 105.0f
	};

	union vec2 scorePos = {
		.x = 20.0f,
		.y = 100.0f
	};
	union vec2 scoreDim = {
		.x = 150.0f,
		.y = 25.0f
	};

	uint32_t scoreCount = CountEntitiesByType(state, ENTTYPE_SCORELABEL);
	uint32_t newPosition = 1;
	float positionOffset = 0;
	if(scoreCount != 0) {
		struct entity *scores[scoreCount];
		uint32_t currentCount = 0;
		for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {
			if(state->entities[i].type == ENTTYPE_SCORELABEL) {
				scores[currentCount++] = &state->entities[i];
			}
		}
		assert(currentCount == scoreCount);
		int32_t i = 0;
		int32_t atPosition = 1;
		for(; i < scoreCount; ++i) {
			if(scores[i]->string.length > state->wpm) {
				atPosition++;
				continue;
			} else {
				scores[i]->string.position++;
				BITCLEAR(scores[i]->state, ENTSTATE_DRAWONCE);
				struct entity_event *event = NewEvent(state, scores[i]->index, EVENT_MOVEDOWN);
				CreateScore(scores[i], scores[i]->string.length, scores[i]->string.position, CHARSTATE_NEUTRAL);
				NewMoveEvent(state, event, FloatToVec2(scores[i]->pos.x, scores[i]->pos.y - 30.0f),
					FloatToVec2(0, -3.5f));
			}
		}
		newPosition = atPosition;
		positionOffset = 30.0f * (newPosition - 1);
	}
	struct entity *newScore = NewEntity(state, scorePos, scoreDim, ENTTYPE_SCORELABEL);
	struct entity *backBox = GetEntityByAlias(state, ENTALIAS_SCOREBOARDBACK);
	newScore->clipRect = MakeClipRect(
		backBox->pos.x, backBox->pos.y,
		backBox->dim.x, backBox->dim.y);
	BITSET(newScore->state, ENTSTATE_ISCLIPPED);
	CreateScore(newScore, state->wpm, newPosition, CHARSTATE_NEUTRAL);
	struct entity_event *event = NewEvent(state, newScore->index, EVENT_MOVEUP);
	scoreBasePos.y -= positionOffset;
	NewMoveEvent(state, event, FloatToVec2(scorePos.x, scoreBasePos.y), FloatToVec2(0, 4.5f));
}


static void ReInitOutput(
	struct game_state *state)
{
	RePositionOutput(state);
	state->atLine = 0;
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
	state->wpm = 0;
	state->correctWords = 0;
	state->correctWordsCarry = 0;
	BITCLEAR(GetEntityByAlias(state, ENTALIAS_OUTPUTRECT)->state, ENTSTATE_WASDRAWN);
	DeleteAllEntitiesOfType(state, ENTTYPE_OUTPUTSTRING);
	DeleteAllEntitiesOfType(state, ENTTYPE_PROGRESSRECT);
	ClearInput(state);
	CreateOutputEntities(state, bufferX, bufferY);
	state->atLine = 0;
}

extern void NewGame(
	struct game_state *state)
{
	InitTimerRects(state);
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

	state->timer.gameStartTime = state->timer.currentTime;
	state->timer.gameLength = SECOND * 60;
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
	BITCLEAR(GetEntityByAlias(state, ENTALIAS_WPMSCORE)->state, ENTSTATE_WASDRAWN);
	BITCLEAR(GetEntityByAlias(state, ENTALIAS_ACCSCORE)->state, ENTSTATE_WASDRAWN);
	BITCLEAR(GetEntityByAlias(state, ENTALIAS_FUNSCORE)->state, ENTSTATE_WASDRAWN);
	RedrawOutput(state);
	NewScore(state);
}

extern void CompareInput(
	struct game_state *state,
	struct entity *inputString)
{
	struct entity *line = GetCurrentOutputLine(state);
	bool completeWord = true;
	uint32_t correctWords = 0;
	uint32_t wrongLetters = 0;
	uint32_t lettersTyped = 0;
	RedrawOutput(state);
	for(int32_t i = 0; i < line->string.length; ++i) {
		char input = inputString->string.contents[i].glyph;
		char output = line->string.contents[i].glyph;
		bool correctLetter = (input == output);
		if(i < inputString->string.length) {
			if(input == ' ' || line->string.contents[i + 1].glyph == '\0') {
				if(completeWord) {
					correctWords++;
				}
				completeWord = true;
			}
			if(!correctLetter) {
				wrongLetters++;
				completeWord = false;
				line->string.contents[i].state = CHARSTATE_WRONG;
			} else {
				line->string.contents[i].state = CHARSTATE_RIGHT;
			}
		}
		if(inputString->string.length <= i) {
			line->string.contents[i].state = CHARSTATE_NEUTRAL;
		}
		lettersTyped = i;
	}
	state->correctWords = correctWords;
	state->lettersWrong = wrongLetters;
	state->lettersTyped = lettersTyped;
}

static void UpdateTime(
	struct game_timer *timer)
{
	timer->currentTime = (api.GetTime() - timer->baseTime);
}

static void ProcessEvent(
	struct game_state *state)
{
	for(int32_t i = 0; i < 24; ++i) {
		struct entity_event *event = &state->events[i];
		struct entity *target = &state->entities[event->parentIndex];
		switch(event->type) {
		case EVENT_BLINK: {
#define BLINK_TIME 500000ull
			if(state->timer.currentTime - event->blink.start > BLINK_TIME) {
				BITTOGGLE(target->state, ENTSTATE_INVISIBLE);
				event->blink.start = state->timer.currentTime;
			}
			break;
		} case EVENT_FADE: {
			if(state->timer.currentTime - event->fade.start > event->fade.end) {
				BITTOGGLE(target->state, ENTSTATE_INVISIBLE);
				event->blink.start = state->timer.currentTime;
				DeleteEvent(state, event->index);
			}
			break;
		} case EVENT_FLASH: {
			if(event->flash.duration != 0 && state->timer.currentTime - event->flash.startTime >
			   event->flash.duration) {
				DeleteEvent(state, event->index);
				ChangeLabelState(target, CHARSTATE_BLACK);
			} else if(state->timer.currentTime - event->flash.lastFlash > event->flash.interval) {
				event->flash.lastFlash = state->timer.currentTime;
				if(target->string.contents[0].state == CHARSTATE_NEUTRAL) {
					ChangeLabelState(target, CHARSTATE_BLACK);
				} else {
					ChangeLabelState(target, CHARSTATE_NEUTRAL);
				}
			}
			break;
		} case EVENT_MOVEUP:
		case EVENT_MOVEDOWN:
		case EVENT_MOVELEFT: {
			target->pos = AddVec2(target->pos, event->move.increment);
			BITCLEAR(target->state, ENTSTATE_WASDRAWN);
			if(event->move.CheckProgress(target->pos, event->move.destination)) {
				target->pos = event->move.destination;
				DeleteEvent(state, event->index);
				if(target->type == ENTTYPE_SCORELABEL) {
					BITSET(target->state, ENTSTATE_DRAWONCE);
					if(target->string.position == 7) {
						DeleteEntityAt(state, target->index);
					}
				}
			}
			if(target->type == ENTTYPE_OUTPUTSTRING) {
				BITCLEAR(state->entities[ENTALIAS_OUTPUTRECT].state, ENTSTATE_WASDRAWN);
			} else if(target->type == ENTTYPE_SCORELABEL) {
				for(int32_t i = ENTALIAS_SCOREBOARDBACK; i <= ENTALIAS_SCORELINE6; ++i) {
					BITCLEAR(state->entities[i].state, ENTSTATE_WASDRAWN);
				}
				RedrawAllScores(state);
			}
			break;
		} default: break;
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
		PushRect(state, current->pos, current->dim, colour);
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
	if(BITCHECK(state->global, GLOBAL_GAME) && !HasActiveEvent(state, current->index)) {
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
			if(state->wpm < 60) {
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
	sprintf(buffer, "%.0f%%", state->accuracey);
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
	sprintf(buffer, "%d", state->wpm);
	CreateLabel(current, buffer, CHARSTATE_UI);
}

static void ProcessEntities(
	struct game_state *state)
{
	for(int32_t i = 0; i < state->entityCount; i++) {
		struct entity *current = &state->entities[i];
		switch(current->type) {
		case ENTTYPE_SCORELABEL:
		case ENTTYPE_LABELSTRING:
		case ENTTYPE_OUTPUTSTRING: {
			DrawString(state, current);
			break;
		} case ENTTYPE_INPUTSTRING: {
			InputControl(state, current);
			DrawString(state, current);
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
			PushRect(state, newPos, current->dim, colour);
			break;
		} case ENTTYPE_GENERICRECT:
		case ENTTYPE_PROGRESSRECT: {
			DrawRect(state, current);
			break;
		} case ENTTYPE_WPMSTRING: {
			if(BITCHECK(state->global, GLOBAL_GAME)) {
				PrepWpmString(state, current);
			}
			DrawString(state, current);
			break;
		} case ENTTYPE_ACCSTRING: {
			if(BITCHECK(state->global, GLOBAL_GAME)) {
				PrepAccString(state, current);
			}
			DrawString(state, current);
			break;
		} default: break;
		}
	}
}

static uint32_t GetWpm(
	struct game_timer *timer,
	uint32_t correctWords)
{
	float minsElapsed = (float)(timer->currentTime - timer->gameStartTime) / 60000000.0f;
	uint32_t result = (float)correctWords / minsElapsed;
	return(result);
}

static float GetAcc(
	struct game_state *state)
{
	float max = state->lettersTyped + state->lettersTypedCarry;
	float value = max - (state->lettersWrong + state->lettersWrongCarry);
	float result = (float)value / (float)max * 100.0f;
	return(result);
}

static void UpdateAcc(
	struct game_state *state)
{
	state->accuracey = GetAcc(state);
	struct entity *accBar = GetEntityByAlias(state, ENTALIAS_ACCBAR);
	accBar->dim.x = (float)state->accuracey * 2.5f;
	struct entity *accBarEnd = GetEntityByAlias(state, ENTALIAS_ACCBAREND);
	accBarEnd->pos.x = accBar->dim.x + accBar->pos.x;
}

static void UpdateWPM(
	struct game_state *state)
{
	state->wpm = GetWpm(&state->timer, state->correctWords + state->correctWordsCarry);
	struct entity *wpmBar = GetEntityByAlias(state, ENTALIAS_WPMBAR);
	wpmBar->dim.x = (float)state->wpm > 100.0f ? 100.0f * 2.5f : (float)state->wpm * 2.5f;
	struct entity *wpmBarEnd = GetEntityByAlias(state, ENTALIAS_WPMBAREND);
	wpmBarEnd->pos.x = wpmBar->dim.x + wpmBar->pos.x;
}

extern void GameLoop(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	if(!BITCHECK(state->global, GLOBAL_INIT)) {
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
	UpdateTime(&state->timer);
	ProcessEvent(state);
	ProcessEntities(state);
	if(BITCHECK(state->global, GLOBAL_GAME)) {
		if(state->timer.currentTime - state->timer.gameStartTime > state->timer.gameLength) {
			EndGame(state);
		} else {
			CompareInput(state, GetEntityByAlias(state, ENTALIAS_INPUTSTRING));
			UpdateWPM(state);
			UpdateAcc(state);
		}
	}
	ProcessRenderJobs(state, buffer);
}