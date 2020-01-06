#pragma once
#ifndef TYPED_ASSETS_H
#define TYPED_ASSETS_H

struct loaded_character {
	int32_t x;
	int32_t y;
	int32_t alignX;
	int32_t alignY;
	int32_t stride;

	void* data;
};

struct character_header {
	size_t size;
	uint32_t startIndex;
};

#endif