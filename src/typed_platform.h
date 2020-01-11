#pragma once
#ifndef TYPED_PLATFORM_H
#define TYPED_PLATFORM_H

/********************************************************************************
 _____                      _ 
/__   \_   _ _ __   ___  __| |		Platform header
  / /\/ | | | '_ \ / _ \/ _` |
 / /  | |_| | |_) |  __/ (_| |
 \/    \__, | .__/ \___|\__,_|
       |___/|_|               
********************************************************************************/

#define BYTES_PER_PIXEL 4
#define SPACE_WIDTH 10.0f
#define SECOND 1000000ull
#define INVALID_PATH assert(0)

#define KILOBYTES(value) ((value)*1024LL)
#define MEGABYTES(value) (KILOBYTES(value)*1024LL)

#define COL_GREEN_DARK FloatToVec4(0.0f, 0.2f, 0.0f, 1.0f)
#define COL_GREEN FloatToVec4(0.3f, 0.65f, 0.3f, 1.0f)
#define COL_GREEN_LIGHT FloatToVec4(0.53f, 0.9f, 0.53f, 1.0f)
#define COL_BLACK FloatToVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define COL_WHITE FloatToVec4(0.8f, 0.8f, 0.8f, 1.0f);
#define COL_RED FloatToVec4(0.8f, 0.3f, 0.4f, 1.0f);
#define COL_ORANGE FloatToVec4(0.75f, 0.7f, 0.3f, 1.0f)
#define COL_ORANGE_DARK FloatToVec4(0.55f, 0.5f, 0.1f, 1.0f)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

static int32_t bufferX;
static int32_t bufferY;

#define PLATFORM_READ_FILE(name) struct file_read_output name(char *fileName)
typedef PLATFORM_READ_FILE(platform_read_file);

#define PLATFORM_GET_TIME(name) uint64_t name(void)
typedef PLATFORM_GET_TIME(platform_get_time);

#define PLATFORM_ALLOCATE(name) char *name(size_t size)
typedef PLATFORM_ALLOCATE(platform_allocate);

#define PLATFORM_DEALLOCATE(name) void name(char *data)
typedef PLATFORM_DEALLOCATE(platform_deallocate);

struct platform_api {
	platform_read_file *ReadFile;
	platform_get_time *GetTime;
	platform_allocate *Allocate;
	platform_deallocate *DeAllocate;
} api;

struct file_read_output {
	uint32_t contentsSize;
	
	void* contents;
};

#endif
