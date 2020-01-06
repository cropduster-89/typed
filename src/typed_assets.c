/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |
  \/    \__, | .__/ \___|\__,_|
        |___/|_|               
********************************************************************************/

#define GLYPH_OFFSET 33

extern void LoadFont(
	uint8_t* charBuffer,
	char *fileName)
{
	int32_t charCount = '~' - '!';

	struct file_read_output file = api.ReadFile(fileName);
	stbtt_fontinfo font;
	stbtt_InitFont(&font, (uint8_t*)file.contents,
		stbtt_GetFontOffsetForIndex((uint8_t*)file.contents, 0));
	uint32_t sizeTotal = sizeof(struct character_header) * charCount;

	for(int32_t i = '!'; i < '~'; ++i) {
		struct loaded_character* result = 
			(struct loaded_character*)(charBuffer + sizeTotal);
		uint8_t *monoBmp = stbtt_GetCodepointBitmap(&font, 0,
			stbtt_ScaleForPixelHeight(&font, 24.0f), i,
			&result->x, &result->y, &result->alignX, &result->alignY);
		assert(monoBmp);
		
		uint32_t size = result->x * result->y * 
			BYTES_PER_PIXEL + sizeof(struct loaded_character);
		((struct character_header*)charBuffer)[i - GLYPH_OFFSET].size = size;
		((struct character_header*)charBuffer)[i - GLYPH_OFFSET].startIndex = sizeTotal;
		sizeTotal += sizeof(struct loaded_character);
		result->data = charBuffer + sizeTotal;
		result->stride = result->x * BYTES_PER_PIXEL;

		uint8_t* src = monoBmp;
		uint8_t* destRow = (uint8_t*)result->data + (result->y - 1) * result->stride;
		for(int32_t y = 0; y < result->y; ++y) {
			uint32_t* dest = (uint32_t*)destRow;
			for(int32_t x = 0; x < result->x; ++x) {
				uint8_t alpha = *src++;
				*dest++ = ((alpha << 24) |
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

extern struct loaded_character *GetCharacter(
	char input,
	void *charBuffer)
{
	assert(input != '\0');
	struct character_header *header = 
		((struct character_header *)charBuffer) + (input - GLYPH_OFFSET);	
	struct loaded_character *character = 
		(struct loaded_character*)((uint8_t *)charBuffer + header->startIndex);
	return(character);
}