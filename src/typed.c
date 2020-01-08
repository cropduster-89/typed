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
			if(scores[i]->string.length > state->score.wpm) {
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
	newScore->string.backgroundIndex = ENTALIAS_SCOREBOARDBACK;
	newScore->string.backgroundCount = ENTALIAS_SCORELINE6 - ENTALIAS_SCOREBOARDBACK;
	struct entity *backBox = GetEntityByAlias(state, ENTALIAS_SCOREBOARDBACK);
	newScore->clipRect = MakeClipRect(
		backBox->pos.x, backBox->pos.y,
		backBox->dim.x, backBox->dim.y);
	BITSET(newScore->state, ENTSTATE_ISCLIPPED);
	CreateScore(newScore, state->score.wpm, newPosition, CHARSTATE_NEUTRAL);
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

extern void ComputeCorrectWords(
	struct game_state *state,
	struct entity *inputString)
{
	struct entity *line = GetCurrentOutputLine(state);
	bool completeWord = true;
	uint32_t correctWords = 0;	
	for(int32_t i = 0; i < line->string.length; ++i) {
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

extern void CompareInput(
	struct game_state *state,
	struct entity *inputString)
{	
	char input = state->input.inputCharacter;
	struct entity *line = GetCurrentOutputLine(state);
	char output = line->string.contents[inputString->string.length].glyph;
	bool currentWord = (input == output);
	struct entity_character *current = &line->string.contents[inputString->string.length];	
	if(currentWord) {
		inputString->string.contents[inputString->string.length++].glyph =
			state->input.inputCharacter;
		inputString->string.lengthInPixels += GetCharacterWidth(state,
			state->input.inputCharacter);		
		IncrementProgressRect(state, inputString);
		if(current->state == CHARSTATE_NEUTRAL) {
			current->state = CHARSTATE_RIGHT;
		}			
	} else {
		current->state = CHARSTATE_WRONG;
	}		
}	

static void UpdateTime(
	struct game_timer *timer)
{
	timer->currentTime = (api.GetTime() - timer->baseTime);
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

static void ProcessEntities(
	struct game_state *state)
{
	for(int32_t i = 0; i < MAX_ENTITIES; i++) {
		struct entity *current = &state->entities[i];
		switch(current->type) {
		case ENTTYPE_SCORELABEL:
		case ENTTYPE_LABELSTRING:
		case ENTTYPE_INPUTSTRING: {
			DrawString(state, current);
			break;
		} case ENTTYPE_OUTPUTSTRING: {
			if(DrawOutput(state, current)) {
				continue;
			}
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

extern uint32_t GetWpm(
	struct game_timer *timer,
	uint32_t correctWords)
{
	float minsElapsed = (float)(timer->currentTime - timer->gameStartTime) / 60000000.0f;
	uint32_t result = (float)correctWords / minsElapsed;
	return(result);
}

extern float GetAcc(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	float max = score->lettersTyped + score->lettersTypedCarry;
	float value = max - (score->lettersWrong + score->lettersWrongCarry);
	float result = (float)value / (float)max * 100.0f;
	return(result);
}

static void UpdateAcc(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	score->accuracey = GetAcc(state);
	struct entity *accBar = GetEntityByAlias(state, ENTALIAS_ACCBAR);
	accBar->dim.x = (float)score->accuracey * 2.5f;
	struct entity *accBarEnd = GetEntityByAlias(state, ENTALIAS_ACCBAREND);
	accBarEnd->pos.x = accBar->dim.x + accBar->pos.x;
}

static void UpdateWPM(
	struct game_state *state)
{
	struct game_score *score = &state->score;
	score->wpm = GetWpm(&state->timer, score->correctWords + score->correctWordsCarry);
	struct entity *wpmBar = GetEntityByAlias(state, ENTALIAS_WPMBAR);
	wpmBar->dim.x = (float)score->wpm > 100.0f ? 100.0f * 2.5f : (float)score->wpm * 2.5f;
	struct entity *wpmBarEnd = GetEntityByAlias(state, ENTALIAS_WPMBAREND);
	wpmBarEnd->pos.x = wpmBar->dim.x + wpmBar->pos.x;
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
	DeleteAllEntitiesOfType(state, ENTTYPE_PROGRESSRECT);
	
	ClearInput(state);
	CreateOutputEntities(state, bufferX, bufferY);	
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

extern void GameLoop(
	struct game_state *state,
	struct screen_buffer *buffer)
{
	if(!BITCHECK(state->global, GLOBAL_INIT)) {
		InitState(state, buffer);
	}
	UpdateTime(&state->timer);
	ReceiveInput(state, GetEntityByAlias(state, ENTALIAS_INPUTSTRING));	
	if(BITCHECK(state->global, GLOBAL_GAME)) {
		if(state->timer.currentTime - state->timer.gameStartTime > state->timer.gameLength) {
			EndGame(state);
		} else {			
			if(state->input.inputCharacter != '\0') {
				CompareInput(state, GetEntityByAlias(state, ENTALIAS_INPUTSTRING));
				ComputeCorrectWords(state, GetEntityByAlias(state, ENTALIAS_INPUTSTRING));
				RedrawAll(state);
			}
			UpdateWPM(state);
			UpdateAcc(state);
		}
	}
	state->input.inputCharacter = '\0';
	ProcessEvent(state);
	ProcessEntities(state);
	ProcessRenderJobs(state, buffer);
}