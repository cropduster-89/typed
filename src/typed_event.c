/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Events implementation
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		TODO change creation so events can be made with 
 \/    \__, | .__/ \___|\__,_|		a single function call 
       |___/|_|               		
					
********************************************************************************/

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

extern bool FreeEvent(
	struct game_state *state)
{
	return(state->eventCount < state->outputLines);
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
	struct entity_event *event,
	uint32_t interval)
{
	event->blink.startTime = timer->currentTime;
	event->blink.interval = interval;
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
	} case EVENT_MOVELEFT: {
		InitMoveEvent(state, event, destination, increment);
		event->move.CheckProgress = event_ProgressCheckLeft;
		break;
	} default: 
#ifdef DEBUG	
	INVALID_PATH;
#else
	break;
#endif
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
#ifdef DEBUG
	assert(event);
#endif	
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

static void ProcessBlink(
	struct game_state *state,
	struct entity_event *event,
	struct entity *target)
{
	if(state->timer.currentTime - event->blink.startTime > event->blink.interval) {
		BITTOGGLE(target->state, ENTSTATE_INVISIBLE);
		event->blink.startTime = state->timer.currentTime;
	}
}

static void ProcessFade(
	struct game_state *state,
	struct entity_event *event,
	struct entity *target)
{
	if(state->timer.currentTime - event->blink.startTime > event->blink.interval) {
		BITSET(target->state, ENTSTATE_INVISIBLE);
		event->blink.startTime = state->timer.currentTime;
	}
}

static void ProcessFlash(
	struct game_state *state,
	struct entity_event *event,
	struct entity *target)
{
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
}

static void ProcessMove(
	struct game_state *state,
	struct entity_event *event,
	struct entity *target)
{
	target->pos = AddVec2(target->pos, event->move.increment);
	if(event->move.CheckProgress(target->pos, event->move.destination)) {
		target->pos = event->move.destination;
		DeleteEvent(state, event->index);
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
			ProcessBlink(state, event, target);
			break;
		} case EVENT_FADE: {
			ProcessFade(state, event, target);
			break;
		} case EVENT_FLASH: {
			ProcessFlash(state, event, target);
			break;
		} case EVENT_MOVEUP:
		case EVENT_MOVEDOWN:
		case EVENT_MOVELEFT: {
			ProcessMove(state, event, target);
			break;
		} default: break;
		}
	}
}