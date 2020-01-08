static bool IsCorrectEntity(
	struct game_state *state,
	int32_t i,
	enum entity_type type,
	bool fromLine)
{
	return(state->entities[i].type == type && (!fromLine || 
	       state->entities[i].string.position == state->atLine));
}

#define GetCurrentOutputLine(state) _GetDynamicEntity(state, ENTTYPE_OUTPUTSTRING, true)
#define GetProgressRect(state) _GetDynamicEntity(state, ENTTYPE_PROGRESSRECT, false)
extern struct entity *_GetDynamicEntity(
	struct game_state *state,
	enum entity_type type,
	bool fromLine)
{
	struct entity *result = NULL;
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {		
		if(IsCorrectEntity(state, i, type, fromLine)) {
			result = &state->entities[i];
			break;
		}
	}
	assert(result);
	return(result);
}

extern bool IsString(
	struct entity *current)
{
	return(current->type > ENTTYPE_NULL && current->type < RECT_START);
}

#define RedrawAllInRange(state, ent) _RedrawEntities(state, \
	ent.string.backgroundIndex, ent.string.backgroundIndex + ent.string.backgroundCount, false)
#define RedrawAllDynamic(state) (_RedrawEntities(state, ENTALIAS_DYNAMICSTART, MAX_ENTITIES, false))
#define RedrawAll(state) (_RedrawEntities(state, 0, MAX_ENTITIES, true))
extern void _RedrawEntities(
	struct game_state *state,
	uint32_t start,
	uint32_t end,
	bool clearAll)
{
	if(!clearAll && IsString(&state->entities[start]) && state->entities[start].string.backgroundIndex != 0) {
		RedrawAllInRange(state, state->entities[start]);
	}
	for(int32_t i = start; i < end; ++i) {		
		BITCLEAR(state->entities[i].state, ENTSTATE_WASDRAWN);
	}
}

extern uint32_t CountEntitiesByType(
	struct game_state *state,
	enum entity_type type)
{
	uint32_t result = 0;
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {
		if(state->entities[i].type == type) {
			result++;
		}
	}
	return(result);
}

extern bool FreeEvent(
	struct game_state *state)
{
	return(state->eventCount < state->outputLines);
}

extern struct entity *GetEntityByAlias(
	struct game_state *state,	
	enum entity_alias alias)
{
	return(&state->entities[alias]);
}

static void ConvertString(
	char *start,
	char *end,
	struct entity_string *entity,
	enum character_state state)
{
	entity->length = end - start;
	assert(entity->length < ARRAY_COUNT(entity->contents));
	char *current = start;
	for(int32_t i = 0; i < entity->length; ++i, ++current) {
		entity->contents[i].glyph = *current;
		entity->contents[i].state = state;
	}
}

extern void DeleteEntityAt(
	struct game_state *state,	
	int32_t i)
{
	struct entity nullEntity = {};
	state->entities[i] = nullEntity;
	state->entityCount--;
}


extern void DeleteAllEntitiesOfType(
	struct game_state *state,
	enum entity_type type)
{
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {
		if(state->entities[i].type == type) {
			DeleteEntityAt(state, i);
		}
	}
}

extern struct entity *NewEntity(
	struct game_state *state,
	union vec2 pos,
	union vec2 dim,
	enum entity_type type)
{
	struct entity *newEntity = NULL;
	for(int32_t i = 0; i < MAX_ENTITIES; ++i) {
		struct entity *current = &state->entities[i];
		if(current->type == ENTTYPE_NULL) {
			newEntity = current;
			newEntity->index = i;
			state->entityCount++;
			break;
		}
	}
	assert(newEntity);

	newEntity->pos = pos;
	newEntity->dim = dim;
	newEntity->type = type;	
	return(newEntity);
}

extern void ChangeLabelState(
	struct entity *label,
	enum character_state state)
{	
	for(int32_t i = 0; i != label->string.length; ++i) {	
		label->string.contents[i].state = state;		
	}
}

extern void CreateScore(
	struct entity *newEntity,
	uint32_t score,
	uint32_t position,
	enum character_state state)
{
	char buffer[10];
	sprintf(buffer, "%d,    %d", position, score);
	
	int32_t i = 0;
	for(char *current = &buffer[0]; *current != '\0'; ++current, ++i) {
		newEntity->string.contents[i].glyph = *current;		
		newEntity->string.contents[i].state = state;		
	}
	newEntity->string.length = score;
	newEntity->string.position = position;
}

extern void CreateLabel(
	struct entity *newEntity,
	char *content,
	enum character_state state)
{
	int32_t i = 0;
	for(char *current = content; *current != '\0'; ++current, ++i) {
		newEntity->string.contents[i].glyph = *current;		
		newEntity->string.contents[i].state = state;		
	}
	newEntity->string.length = strlen(content);
}

EVENT_CHECK_PROGRESS(event_ProgressCheckUp)
{
	return(pos.y > destination.y);
}

EVENT_CHECK_PROGRESS(event_ProgressCheckDown)
{
	return(pos.y < destination.y);
}

EVENT_CHECK_PROGRESS(event_ProgressCheckLeft)
{
	return(pos.x < destination.x);
}

extern void NewFlashEvent(
	struct game_timer *timer,
	struct entity_event *event,
	uint64_t duration,
	uint64_t interval)
{
	event->flash.lastFlash = timer->currentTime;
	event->flash.duration = duration;
	event->flash.interval = interval;
	if(duration != 0) {
		event->flash.startTime = timer->currentTime;
	}
}

static void NewFadeEvent(
	struct game_timer *timer,
	struct entity_event *event,
	uint64_t fadeTime)
{
	event->fade.start = timer->currentTime;
	event->fade.end = fadeTime;
}

static void NewBlinkEvent(
	struct game_timer *timer,
	struct entity_event *event)
{
	event->blink.start = timer->currentTime;
}

static void InitMoveEvent(
	struct game_state *state,
	struct entity_event *event,
	union vec2 destination,
	union vec2 increment)
{
	event->move.origin = state->entities[event->parentIndex].pos;
	event->move.destination = destination;
	event->move.increment = increment;
}

static void NewMoveEvent(
	struct game_state *state,
	struct entity_event *event,
	union vec2 destination,
	union vec2 increment)
{
	switch(event->type) {
	case EVENT_MOVEUP: {
		InitMoveEvent(state, event, destination, increment);
		event->move.CheckProgress = event_ProgressCheckUp;
		break;
	} case EVENT_MOVEDOWN: {
		InitMoveEvent(state, event, destination, increment);
		event->move.CheckProgress = event_ProgressCheckDown;
		break;
	}case EVENT_MOVELEFT: {
		InitMoveEvent(state, event, destination, increment);
		event->move.CheckProgress = event_ProgressCheckLeft;
		break;
	} default: INVALID_PATH;		
	}	
}

extern struct entity_event *NewEvent(
	struct game_state *state,
	int32_t parentIndex,
	enum event_types type)
{
	struct entity_event *event = NULL;
	for(int32_t i = 0; i < MAX_EVENTS; ++i) {
		if(state->events[i].type == EVENT_NULL) {
			event = &state->events[i];
			event->index = i;
			state->eventCount++;
			break;
		}
	}
	assert(event);
	
	event->type = type;
	event->parentIndex = parentIndex;
	return(event);	
}

static void DeleteEvent(
	struct game_state *state,
	uint32_t index)
{
	struct entity_event nullEvent = {};
	assert(state->events[index].type != EVENT_NULL);
	state->events[index] = nullEvent;
	state->eventCount--;
}

static void DeleteAllEventsByType(
	struct game_state *state,
	enum event_types type)
{
	for(int32_t i = 0; i < MAX_EVENTS; ++i) {
		if(state->events[i].type == type) {
			DeleteEvent(state, i);
		}
	}
}

extern bool HasActiveEvent(
	struct game_state *state,
	uint32_t index)
{
	bool result = false;
	for(int32_t i = 0; i < MAX_EVENTS; ++i) {
		if(state->events[i].parentIndex == index) {
			result = true;
			break;
		}
	}
	return(result);
}

extern void RePositionOutput(
	struct game_state *state)
{
	union vec2 pos = FloatToVec2(
		(float)bufferX * 0.25f,
		(float)bufferY - 110.0f);

	uint32_t count = 0;
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {		
		if(state->entities[i].type == ENTTYPE_OUTPUTSTRING) {
			state->entities[i].pos.y = pos.y - 25.0f * count;
			count++;			
		}
	}
}

extern void ProcessEvent(
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
			if(target->index >= ENTALIAS_DYNAMICSTART) {
				RedrawAllDynamic(state);
			}
			break;
		} default: break;
		}
	}
}

static void ScrollOutput(
	struct game_state *state,
	bool isDown)
{
	for(int32_t i = ENTALIAS_DYNAMICSTART; i < MAX_ENTITIES; ++i) {
		struct entity *current = &state->entities[i];
		if(current->type == ENTTYPE_OUTPUTSTRING) {			
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
	}
}