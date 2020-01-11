/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		*Asset implementation
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |		*loading bitmaps, accessing bitmaps
  \/    \__, | .__/ \___|\__,_|
        |___/|_|               
********************************************************************************/

#define GLYPH_OFFSET 33
#define SCALE 0.5f 

extern void LoadFont(
	uint8_t* charBuffer,
	char *fileName)
{
	int32_t charCount = '~' - '!';

	struct file_read_output file = api.ReadFile(fileName);
	stbtt_fontinfo font;
	stbtt_InitFont(&font, (uint8_t*)file.contents,
		stbtt_GetFontOffsetForIndex((uint8_t*)file.contents, 0));
	uint32_t sizeTotal = sizeof(struct asset_header) * charCount;

	for(int32_t i = '!'; i < '~'; ++i) {
		struct loaded_character* result = 
			(struct loaded_character*)(charBuffer + sizeTotal);
		uint8_t *monoBmp = stbtt_GetCodepointBitmap(&font, 0,
			stbtt_ScaleForPixelHeight(&font, 48.0f), i,
			&result->x, &result->y, &result->alignX, &result->alignY);
#ifdef DEBUG
		assert(monoBmp);
#endif		
		uint32_t size = result->x * result->y * 
			BYTES_PER_PIXEL + sizeof(struct loaded_character);
		((struct asset_header*)charBuffer)[i - GLYPH_OFFSET].size = size;
		((struct asset_header*)charBuffer)[i - GLYPH_OFFSET].startIndex = sizeTotal;
		sizeTotal += sizeof(struct loaded_character);
		result->data = charBuffer + sizeTotal;
		result->stride = result->x * BYTES_PER_PIXEL;

		uint8_t *src = monoBmp;
		uint8_t *destRow = (uint8_t *)result->data + (result->y - 1) * result->stride;
		for(int32_t y = 0; y < result->y; ++y) {
			uint32_t* dest = (uint32_t *)destRow;
			for(int32_t x = 0; x < result->x; ++x) {
				uint8_t alpha = *src++;
				*dest++ = (
					(alpha << 24) |
					(alpha << 16) |
					(alpha << 8) |
					(alpha << 0));
			}
			destRow -= result->stride;
		}
		stbtt_FreeBitmap(monoBmp, 0);
		sizeTotal += size;
	}
}

extern void LoadBmps(
	struct game_state *state,
	int32_t count)
{
	uint32_t sizeTotal = sizeof(struct asset_header) * count;
	int32_t rectIndex = 1;
	int32_t nextStart = 1;
	for(int32_t i = 0; i < count - 1; ++i) {		
		rectIndex = GetNextRect(state, nextStart);
		
		struct entity *rect = &state->entities[rectIndex];
		union vec2 dim = rect->dim;
		
		struct loaded_bmp *result = (struct loaded_bmp *)(state->bmpBuffer + sizeTotal);
		
		uint32_t size = dim.x * dim.y * 
			BYTES_PER_PIXEL + sizeof(struct loaded_bmp);
		((struct asset_header*)state->bmpBuffer)[i].size = size;
		((struct asset_header*)state->bmpBuffer)[i].startIndex = sizeTotal;
		sizeTotal += sizeof(struct loaded_bmp);
		result->x = dim.x;
		result->y = dim.y;
		result->data = state->bmpBuffer + sizeTotal;
		result->stride = result->x * BYTES_PER_PIXEL;
		
		DrawInternalBmp(result->data, dim, rect->rect.colour);
		rect->rect.assetIndex = i;
		
		sizeTotal += size;
		nextStart = rectIndex + 1;		
	}
}

extern struct loaded_character *GetCharacter(
	char input,
	void *charBuffer)
{
#ifdef DEBUG
	assert(input != '\0');
#endif
	struct asset_header *header = 
		((struct asset_header *)charBuffer) + (input - GLYPH_OFFSET);	
	struct loaded_character *character = 
		(struct loaded_character*)((uint8_t *)charBuffer + header->startIndex);
	return(character);
}

extern struct loaded_bmp *GetBmp(
	int32_t index,
	void *bmpBuffer)
{
	struct asset_header *header = 
		((struct asset_header *)bmpBuffer) + (index);	
	struct loaded_bmp *bmp = 
		(struct loaded_bmp*)((uint8_t *)bmpBuffer + header->startIndex);
	assert(bmp);
	return(bmp);
}