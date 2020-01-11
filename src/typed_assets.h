#pragma once
#ifndef TYPED_ASSETS_H
#define TYPED_ASSETS_H

struct loaded_character {
	void *data;
	
	int32_t x;
	int32_t y;	
	int32_t stride;
	
	int32_t alignX;
	int32_t alignY;
};

struct loaded_bmp {
	void *data;
	
	int32_t x;
	int32_t y;	
	int32_t stride;
};

struct asset_header {
	size_t size;
	uint32_t startIndex;
};

#endif