/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		Renderer
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |		*Move solid rects to bmps drawn at runtime
  \/    \__, | .__/ \___|\__,_|		*SIMD! 
        |___/|_|               

********************************************************************************/

static void PushRenderJob(
	struct game_state *state,
	union render_job job)
{
	assert(state->renderJobCount < MAX_RENDERJOBS);
	state->renderJobs[state->renderJobCount++] = job;
}

extern void PushRect(
	struct game_state *state,
	union vec2 pos,
	union vec2 dim,
	union vec4 colour)
{
	union render_job job = {};
	job.rect.pos = pos;
	job.rect.dim = dim;
	job.rect.colour = colour;
	job.rect.type = RJOB_RECT;
	PushRenderJob(state, job);
} 

static void PushLetter(
	struct game_state *state,	
	struct loaded_character *character,
	union vec4 colour,	
	union vec2 pos,
	struct rect2 clipRect)
{
	union render_job job = {};
	job.glyph.pos = pos;
	job.glyph.colour = colour;
	job.glyph.asset = character;
	job.glyph.type = RJOB_LETTER;
	job.glyph.clipRect = clipRect;
	PushRenderJob(state, job);
}

static union vec4 GetCharacterColour(
	struct entity_character *character)
{
	union vec4 result = {};
	
	switch(character->state) {
	case CHARSTATE_BLACK: {
		result = COL_BLACK;
		break;
	} case CHARSTATE_NEUTRAL: {
		result = COL_WHITE;
		break;
	} case CHARSTATE_UI: {
		result = COL_GREEN;
		break;
	} case CHARSTATE_RIGHT: {
		result = COL_GREEN_LIGHT;
		break;
	} case CHARSTATE_WRONG: {
		result = COL_RED;
		break;
	} default: INVALID_PATH;
	}
	
	return(result);
}

extern void PushString(
	struct game_state *state,
	struct entity *entity,
	struct rect2 clipRect)
{
	union vec2 pos = {
		.x = entity->pos.x,
		.y = entity->pos.y,
	};
	
	for(int32_t i = 0; i < entity->string.length; ++i) {
		char glyph = entity->string.contents[i].glyph;
		if(glyph == ' ') {
			pos.x += SPACE_WIDTH;
			continue;
		} else if(glyph == '\0') {
			break;
		}
		struct loaded_character *character = 
			GetCharacter(glyph, state->characterBuffer);
		int32_t alignedY = pos.y - character->y - character->alignY;	
		union vec4 colour = GetCharacterColour(&entity->string.contents[i]);
		
		union vec2 adjustedPos = {
			.x = pos.x,
			.y = alignedY,
		};
		PushLetter(state, character, colour, adjustedPos, clipRect);
		pos.x += character->x; 
	}
}

static bool ClippedPixel(
	uint32_t x,
	uint32_t y,	
	struct rect2 clipRect)
{	
	return(x < clipRect.min.x ||
	       y < clipRect.min.y ||		
	       x > clipRect.max.x - 1 || 
	       y > clipRect.max.y - 1); 
}

static uint32_t UnpackColour(
	union vec4 colour)
{
	uint32_t result = (
		((uint32_t)(colour.a * 255) << 24) |
		((uint32_t)(colour.r * 255) << 16) |
		((uint32_t)(colour.g * 255) << 8) |
		((uint32_t)(colour.b * 255) << 0));
	return(result);
}

static void WriteBmpToBuffer(
	struct screen_buffer *buffer,
	struct loaded_character *character,
	union vec2 start,
	union vec4 colour,
	struct rect2 clipRect)
{
	union vec2 xAxis = FloatToVec2(character->x * 0.95f, 0); 
	union vec2 yAxis = FloatToVec2(0, character->y * 0.95f); 
	float xLengthIverted = 1.0f / LengthSqVec2(xAxis);
	float yLengthIverted = 1.0f / LengthSqVec2(yAxis);
	
	int32_t xMaxInit = clipRect.max.x;
	int32_t yMaxInit = clipRect.max.y;
	int32_t xMin = xMaxInit;
	int32_t yMin = yMaxInit;
	int32_t xMax = clipRect.min.x;
	int32_t yMax = clipRect.min.y;
	
	union vec2 p[] = {
		start,
		AddVec2(xAxis, start),
		AddVec2(AddVec2(xAxis, start), yAxis),
		AddVec2(yAxis, start)		
	};
	for(int32_t i = 0; i < ARRAY_COUNT(p); ++i) {
		union vec2 testPos = p[i];
		int32_t floorX = floorf(testPos.x);
		int32_t ceilX = ceilf(testPos.x);
		int32_t floorY = floorf(testPos.y);
		int32_t ceilY = ceilf(testPos.y);
		
		if(xMin > floorX) {xMin = floorX;}
		if(yMin > floorY) {yMin = floorY;}
		if(xMax < ceilX) {xMax = ceilX;}
		if(yMax < ceilY) {yMax = ceilY;}
	}
	if(xMin < 0) {xMin = 0;}
	if(yMin < 0) {yMin = 0;}
	if(xMax > xMaxInit) {xMax = xMaxInit;}
	if(yMax > yMaxInit) {yMax = yMaxInit;}
	
	uint8_t *row = ((uint8_t *)buffer->data + 
		xMin * BYTES_PER_PIXEL + yMin * buffer->stride);
	
	for(int32_t y = yMin; y <= yMax; ++y) {	
		uint32_t *dest = (uint32_t *)row;
		for(int32_t x = xMin; x <= xMax; ++x) {	
			if(ClippedPixel(x, y, clipRect)) {			
				dest++; continue; 
			}
			
			union vec2 pixel = FloatToVec2(x, y);
			union vec2 pixelOffset = SubVec2(pixel, start);	
			
			float edge0 = InnerVec2(pixelOffset, NegVec2(Perp(xAxis)));
			float edge1 = InnerVec2(SubVec2(pixelOffset, xAxis), NegVec2(Perp(yAxis)));
			float edge2 = InnerVec2(SubVec2(SubVec2(pixelOffset, xAxis), yAxis), Perp(xAxis));
			float edge3 = InnerVec2(SubVec2(pixelOffset, yAxis),Perp(yAxis));
			if(edge0 > 0 || edge1 > 0 || edge2 > 0 || edge3 > 0) {dest++; continue;}			
	
			float u = xLengthIverted * InnerVec2(pixelOffset, xAxis) * 1.1f;
			float v = yLengthIverted * InnerVec2(pixelOffset, yAxis) * 1.1f;
			float tX = ((u * (float)(character->x - 2)));
			float tY = ((v * (float)(character->y - 2)));
			int32_t nX = (int32_t)tX;
			int32_t nY = (int32_t)tY;
			float fX = tX - (float)nX;
			float fY = tY - (float)nY;			
			
			assert((nX >= 0) && (nX < character->x));
			assert((nY >= 0) && (nY < character->y));
			
			uint8_t *src = ((uint8_t *)character->data + nY * 
				character->stride + nX * BYTES_PER_PIXEL);
			uint32_t sample0 = *(uint32_t *)(src);
			uint32_t sample1 = *(uint32_t *)(src + BYTES_PER_PIXEL);
			uint32_t sample2 = *(uint32_t *)(src + character->stride);
			uint32_t sample3 = *(uint32_t *)(src + character->stride + BYTES_PER_PIXEL);
			
			float texel0R = (float)((sample0 >> 16) & 0xff);
			float texel0G = (float)((sample0 >> 8) & 0xff);
			float texel0B = (float)((sample0 >> 0) & 0xff);
			float texel0A = (float)((sample0 >> 24) & 0xff);			
			
			float texel1R = (float)((sample1 >> 16) & 0xff);
			float texel1G = (float)((sample1 >> 8) & 0xff);
			float texel1B = (float)((sample1 >> 0) & 0xff);
			float texel1A = (float)((sample1 >> 24) & 0xff);

			float texel2R = (float)((sample2 >> 16) & 0xff);
			float texel2G = (float)((sample2 >> 8) & 0xff);
			float texel2B = (float)((sample2 >> 0) & 0xff);
			float texel2A = (float)((sample2 >> 24) & 0xff);

			float texel3R = (float)((sample3 >> 16) & 0xff);
			float texel3G = (float)((sample3 >> 8) & 0xff);
			float texel3B = (float)((sample3 >> 0) & 0xff);
			float texel3A = (float)((sample3 >> 24) & 0xff);
			
			float ifX = 1.0f - fX;
			float ifY = 1.0f - fY;
		
			float lerp0 = ifY * ifX;
			float lerp1 = ifY * fX;
			float lerp2 = fY * ifX;
			float lerp3 = fY * fX;
			
			float texelR = lerp0 * texel0R + lerp1 * texel1R + lerp2 * texel2R + lerp3 * texel3R;
			float texelG = lerp0 * texel0G + lerp1 * texel1G + lerp2 * texel2G + lerp3 * texel3G;
			float texelB = lerp0 * texel0B + lerp1 * texel1B + lerp2 * texel2B + lerp3 * texel3B;
			float texelA = lerp0 * texel0A + lerp1 * texel1A + lerp2 * texel2A + lerp3 * texel3A;
		
			texelR = texelR * colour.r;
			texelG = texelG * colour.g;
			texelB = texelB * colour.b;
			texelA = texelA * colour.a;
			
			float srcA = texelA;
			float srcR = texelR;
			float srcG = texelG;
			float srcB = texelB;
			float srcACorrected = (srcA / 255.0f) * colour.a;
			
			float destA = (float)((*dest >> 24) & 0xff);
			float destR = (float)((*dest >> 16) & 0xff);
			float destG = (float)((*dest >> 8) & 0xff);
			float destB = (float)((*dest >> 0) & 0xff);			
			float destACorrected = (destA / 255.0f);
			
			float inverseSrcA = (1.0f - srcACorrected);
			float a = 255.0f * (srcACorrected + destACorrected - srcACorrected * destACorrected);
			float r = inverseSrcA * destR + srcR;
			float g = inverseSrcA * destG + srcG;
			float b = inverseSrcA * destB + srcB;
			
			*dest++ = (
				((uint32_t)(a + 0.5f) << 24) |
				((uint32_t)(r + 0.5f) << 16) |
				((uint32_t)(g + 0.5f) << 8) |
				((uint32_t)(b + 0.5f) << 0));	
		}
	row += buffer->stride;
	}
}	

static void WriteSolidColourToBuffer(
	struct screen_buffer* b,
	int32_t x,
	int32_t y,
	uint32_t startX,
	uint32_t startY,
	union vec4 colour)
{
	uint32_t uColour = UnpackColour(colour);
	uint32_t* data = (uint32_t*)b->data + startX + (b->x * startY);
	for (int32_t i = 0; i < x * y; ++i) {
		if (i % x == 0 && i != 0) {
			data += b->x - x;
		}	
		*data++ = uColour;
	}
}

extern void ProcessRenderJobs(
	struct game_state *state,
	struct screen_buffer *b)
{
	for(int32_t i = 0; i < state->renderJobCount; ++i) {
		union render_job *job = &state->renderJobs[i];
		switch(job->glyph.type) {
		case RJOB_LETTER: {			
			WriteBmpToBuffer(b, job->glyph.asset, job->glyph.pos, 
				job->glyph.colour, job->glyph.clipRect);
			break;
		} case RJOB_RECT: {			
			WriteSolidColourToBuffer(b, job->rect.dim.x, job->rect.dim.y, job->rect.pos.x, 
				job->rect.pos.y, job->rect.colour);
			break;
		} default: break;	
		}
	}
	state->renderJobCount = 0;
}