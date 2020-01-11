#pragma once
#ifndef TYPED_RENDER_H
#define TYPED_RENDER_H

/********************************************************************************
  _____                      _ 
 /__   \_   _ _ __   ___  __| |		*Renderer header
   / /\/ | | | '_ \ / _ \/ _` |
  / /  | |_| | |_) |  __/ (_| |
  \/    \__, | .__/ \___|\__,_|
        |___/|_|               
********************************************************************************/

enum render_job_types {
	RJOB_LETTER,
	RJOB_RECT,
	RJOB_BACKRECT
};

struct render_letter {
	enum render_job_types type;
	struct loaded_character *asset;
	union vec2 pos;
	union vec4 colour;
	struct rect2 clipRect;
	bool isClipped;
};

struct render_rect {
	enum render_job_types type;
	struct loaded_bmp *asset;
	union vec2 pos;
	union vec2 dim;
	union vec4 colour;
};

union render_job {
	struct render_rect rect;
	struct render_letter glyph;
};

struct game_state;
struct screen_buffer;
struct entity;

void PushRect(struct game_state *, uint32_t assetIndex, union vec2 pos, union vec2 dim, union vec4 colour);
void PushBackRect(struct game_state *, union vec2 pos, union vec2 dim, union vec4 colour);
void PushString(struct game_state *, struct entity *, struct rect2 clipRect);
void ProcessRenderJobs(struct game_state *, struct screen_buffer *);
void DrawInternalBmp(uint32_t *data, union vec2 dim, union vec4 colour);

#endif