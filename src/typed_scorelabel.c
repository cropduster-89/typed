static int32_t ReArrangeOldScores(
	struct game_state *state,
	int32_t scoreCount)
{
	struct entity *scores[scoreCount];
	uint32_t currentCount = 0;
	for(int32_t i = 0; i < MAX_ENTITIES; ++i) {
		if(state->entities[i].type == ENTTYPE_SCORELABEL) {
			scores[currentCount++] = &state->entities[i];
		}
	}
	assert(currentCount == scoreCount);
	
	int32_t atPosition = 1;
	for(int32_t i = 0; i < scoreCount; ++i) {
		if(scores[i]->string.length > state->score.wpm) {
			atPosition++;
			continue;
		} else {
			scores[i]->string.position++;			
			struct entity_event *event = NewEvent(state, scores[i]->index, EVENT_MOVEDOWN);
			CreateScore(scores[i], scores[i]->string.length, scores[i]->string.position, CHARSTATE_NEUTRAL);
			NewMoveEvent(state, event, FloatToVec2(scores[i]->pos.x, scores[i]->pos.y - 30.0f),
				FloatToVec2(0, -3.5f));
		}
	}
	
	return(atPosition);	
}

extern void NewScore(
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
	
	union vec2 scoreBoardBackPos = {
		.x = 10.0f,
		.y = bufferY - 265.0f
	};
	union vec2 scoreBoardBackDim = {
		.x = bufferX * 0.25f - 70.0f,
		.y = bufferY - 80.0f - scoreBoardBackPos.y
	};	
	
	uint32_t newPosition = 1;
	float positionOffset = 0;
	uint32_t scoreCount = CountEntitiesByType(state, ENTTYPE_SCORELABEL, CountSingleType);
	if(scoreCount != 0) {
		newPosition = ReArrangeOldScores(state, scoreCount);
		positionOffset = 30.0f * (newPosition - 1);
	}	
	
	struct entity *newScore = NewEntity(state, scorePos, scoreDim, ENTTYPE_SCORELABEL);
	newScore->clipRect = MakeClipRect(
		scoreBoardBackPos.x, scoreBoardBackPos.y,
		scoreBoardBackDim.x, scoreBoardBackDim.y);
	BITSET(newScore->state, ENTSTATE_ISCLIPPED);
	CreateScore(newScore, state->score.wpm, newPosition, CHARSTATE_NEUTRAL);
	
	struct entity_event *event = NewEvent(state, newScore->index, EVENT_MOVEUP);
	scoreBasePos.y -= positionOffset;
	NewMoveEvent(state, event, FloatToVec2(scorePos.x, scoreBasePos.y), FloatToVec2(0, 4.5f));
}

extern void DeleteIfObselete(
	struct game_state *state,
	struct entity *current)
{
	if(current->string.position >= 7) {
		DeleteEntityAt(state, current->index);
	}
}