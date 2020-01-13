/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Main implmentation:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*Main game loop 
 \/    \__, | .__/ \___|\__,_|		
       |___/|_|               		
********************************************************************************/

extern void UpdateTime(
	struct game_timer *timer)
{
	timer->currentTime = (api.GetTime() - timer->baseTime);
}

static void DrawBackRect(
	struct game_state *state,
	struct entity *current)
{
	if(!BITCHECK(current->state, ENTSTATE_INVISIBLE)) {		
		PushBackRect(state, current->pos, current->dim, current->rect.colour);
	}
}

static void DrawRect(
	struct game_state *state,
	struct entity *current)
{
	if(!BITCHECK(current->state, ENTSTATE_INVISIBLE)) {		
		PushRect(state, current->rect.assetIndex, current->pos, current->dim, current->rect.colour);
	}
}

static void DrawString(
	struct game_state *state,
	struct entity *current)
{
	if(!BITCHECK(current->state, ENTSTATE_INVISIBLE)) {		
		struct rect2 cliprect = BITCHECK(current->state, ENTSTATE_ISCLIPPED) ?
			current->clipRect : FloatToRect2(0, 0, bufferX, bufferY);
		PushString(state, current, cliprect);
	}
}

static void ProcessEntities(
	struct game_state *state)
{
	for(int32_t i = 0; i < MAX_ENTITIES; i++) {
		struct entity *current = &state->entities[i];
		switch(current->type) {
		case ENTTYPE_SCORELABEL: {
			DeleteIfObselete(state, current);
			DrawString(state, current);
			break;
		} case ENTTYPE_LABELSTRING: {
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
		} case ENTTYPE_CURSORRECT:
		case ENTTYPE_GENERICRECT: {
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
	struct entity *inputString = GetEntityByAlias(state, ENTALIAS_INPUTSTRING);
	struct entity *cursorRect = GetEntityByAlias(state, ENTALIAS_INPUTCURSOR);
	cursorRect->pos.x = inputString->pos.x;
	UpdateWPM(state);
	UpdateAcc(state);
	DeleteAllEntitiesOfType(state, ENTTYPE_OUTPUTSTRING);	
	ClearInput(state);
	CreateOutputEntities(state, bufferX, bufferY);	
}

extern void NewGame(
	struct game_state *state)
{
	InitTimerRects(state);	
	CreateProgressRect(state);
	
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
	struct entity *cursorRect = GetEntityByAlias(state, ENTALIAS_INPUTCURSOR);
	cursorRect->pos.x = inputString->pos.x;
	inputString->string.length = 0;
	inputString->string.lengthInPixels = 0;
	ResetTimerRects(state);
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
	LoadFont(state->characterBuffer, DEFAULT_FONT_PATH);	
	LoadBmps(state, CountEntitiesByType(state, 0, CountRects));
	CreateOutputEntities(state, buffer->x, buffer->y);
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
		} 		
		ComputeScore(state);
		UpdateWPM(state);
		UpdateAcc(state);				
	}	
	ProcessEntities(state);	
	ProcessEvent(state);
	ProcessRenderJobs(state, buffer);
	EndFrame(state);
}