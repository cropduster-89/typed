/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Entity implmentation:
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |		*Support functions for creating,
 \/    \__, | .__/ \___|\__,_|		deleting, and doing unspecified 
       |___/|_|               		things to entities
********************************************************************************/

static bool IsCorrectEntity(
	struct game_state *state,
	int32_t i,
	enum entity_type type,
	bool fromLine)
{
	return(state->entities[i].type == type && (!fromLine || 
	       state->entities[i].string.position == state->atLine));
}

#define GetCurrentOutputLine(state) _GetDynamicEntity(state, \
	ENTTYPE_OUTPUTSTRING, ENTALIAS_DYNAMICSTART, true)
#define GetProgressRect(state) _GetDynamicEntity(state, ENTTYPE_PROGRESSRECT, ENTALIAS_DYNAMICSTART, false)
extern struct entity *_GetDynamicEntity(
	struct game_state *state,
	enum entity_type type,
	int32_t start,
	bool fromLine)
{
	struct entity *result = NULL;
	for(int32_t i = start; i < MAX_ENTITIES; ++i) {		
		if(IsCorrectEntity(state, i, type, fromLine)) {
			result = &state->entities[i];
			break;
		}
	}
#ifdef DEBUG
	assert(result);
#endif
	return(result);
}

#define IsString(entity) _IsType(entity, STRING_START, RECT_START)
#define IsRect(entity) _IsType(entity, RECT_START, ENTTYPE_BACKRECT)
extern bool _IsType(
	struct entity *current,
	int32_t start,
	int32_t end)
{
	return(current->type >= start && current->type < end);
}

ENTITY_COUNT_CONDITION(CountRects)
{
	return(state->entities[i].type >= RECT_START && state->entities[i].type < ENTTYPE_BACKRECT);
}

ENTITY_COUNT_CONDITION(CountSingleType)
{
	return(state->entities[i].type == type);
}

extern uint32_t CountEntitiesByType(
	struct game_state *state,
	enum entity_type type,
	entity_count_condition *condition)
{
	uint32_t result = 0;
	for(int32_t i = 0; i < MAX_ENTITIES; ++i) {
		if(condition(state, type, i)) {
			result++;
		}
	}
	return(result);
}

extern int32_t GetNextRect(
	struct game_state *state,
	int32_t startFrom)
{
	int32_t result = 0;
	for(int32_t i = startFrom; i < MAX_ENTITIES; ++i) {
		if(IsRect(&state->entities[i])) {
			result = state->entities[i].index;
			break;
		}
	}
	return(result);
}

extern struct entity *GetEntityByAlias(
	struct game_state *state,	
	enum entity_alias alias)
{
	return(&state->entities[alias]);
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
#ifdef DEBUG
	assert(newEntity);
#endif
	newEntity->pos = pos;
	newEntity->dim = dim;
	newEntity->type = type;	
	return(newEntity);
}