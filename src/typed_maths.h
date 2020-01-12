#pragma once
#ifndef TYPED_MATHS_H
#define TYPED_MATHS_H

union vec2 {
	struct {
		float x;
		float y;
	};
	float array[2];
};

union vec3 {
	struct {
		float x;
		float y;
		float z;
	};
	struct {
		float r;
		float g;
		float b;
	};
	float array[3];
};

union vec4 {
	struct {
		float x;
		float y;
		float z;
		float w;
	};
	struct {
		float r;
		float g;
		float b;
		float a;
	};
	struct {
		union vec3 xyz;
		float ignored;
	};
	float array[4];
};

struct rect2 {
	union vec2 min;
	union vec2 max;
};



extern float Sqr(
	float a)
{
	return(a * a);
}

extern float Clamp(
	float a,
	float min,
	float max)
{
	a = a < min ? min : a; 
	a = a > max ? max : a;
	return(a);
}

extern float ClampColour(
	float a)
{
	return(Clamp(a, 0.0f, 1.0f));
}

extern union vec4 ClampColourV4(
	union vec4 a)
{
	union vec4 result = {
		.r = ClampColour(a.r),
		.g = ClampColour(a.g),
		.b = ClampColour(a.b),
		.a = ClampColour(a.a),
	};
	return(result);
}

extern union vec2 FloatToVec2(
	float x,
	float y)
{
	union vec2 result = {
		.x = x,
		.y = y
	};
	return(result);
}
#define DEF_V2 FloatToVec2(0, 0)

extern union vec2 Perp(
	union vec2 a)
{
	union vec2 result = {
		.x = -a.y,
		.y = a.x
	};
	return(result);
}

extern union vec2 AddVec2(
	union vec2 a,
	union vec2 b)
{
	union vec2 result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
	}; 
	return(result);
} 

extern union vec2 NegVec2(
	union vec2 a)
{
	union vec2 result = {
		.x = -a.x,
		.y = -a.y,
	}; 
	return(result);
} 

extern union vec2 MultVec2(
	union vec2 a,
	float b)
{
	union vec2 result = {
		.x = a.x * b,
		.y = a.y * b
	};
	return(result);
}

extern union vec2 SubVec2(
	union vec2 a,
	union vec2 b)
{
	union vec2 result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
	}; 
	return(result);
} 

extern float InnerVec2(
	union vec2 a,
	union vec2 b)
{
	return(a.x * b.x + a.y * b.y);
}

extern float LengthVec2(
	union vec2 a)
{
	return(InnerVec2(a, a));
}

extern float LengthSqVec2(
	union vec2 a)
{
	float result = InnerVec2(a, a);
	return(result);
}

extern union vec4 FloatToVec4(
	float x,
	float y,
	float z,
	float w)
{
	union vec4 result = {
		.x = x,
		.y = y,
		.z = z,
		.w = w
	};
	return(result);
}

extern union vec3 MultVec3(
	union vec3 a,
	float b)
{
	union vec3 result = {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b,
	};
	return(result);
}

extern union vec4 MultVec4(
	union vec4 a,
	float b)
{
	union vec4 result = {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b,
		.w = a.w * b
	};
	return(result);
}

extern union vec4 AddVec4(
	union vec4 a,
	union vec4 b)
{
	union vec4 result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
		.w = a.w + b.w
	};
	return(result);
}

extern float LerpFloat(
	float a,
	float b,
	float t)
{
	return((1.0f - t) * a + t * b);
}

extern union vec4 LerpVec4(
	union vec4 a,
	union vec4 b,
	float t)
{
	return(AddVec4(MultVec4(a, 1.0f - t), 
			MultVec4(b, t)));
}

#define LERP(a, b, t) _Generic((a), \
float : LerpFloat, \
union vec4 : LerpVec4) ((a), (b), (t))

extern struct rect2 FloatToRect2(
	float a,
	float b,
	float c,
	float d)
{
	struct rect2 result = {
		.min = {
			.x = a,
			.y = b,
		},
		.max = {
			.x = c,
			.y = d,
		},
	};
	return(result);
}

extern struct rect2 MakeClipRect(
	float a,
	float b,
	float c,
	float d)
{
	struct rect2 result = FloatToRect2(a, b, c, d);
	result.max = AddVec2(result.min, result.max);
	return(result);
}

struct point2 {
	int32_t x;
	int32_t y;
};

extern inline bool _BITCHECK_U32(uint32_t a,
			         int32_t n)
{
	return(a & (1ul << n));
}

extern inline bool _BITCHECK_U64(uint64_t a,
			         int32_t n)
{
	return(a & (1ull << n));
}

#define BITCHECK(a, n) _Generic((a), \
uint32_t : _BITCHECK_U32, \
uint64_t : _BITCHECK_U64) ((a), (n))

extern inline void _BITCLEAR_U32(uint32_t *a,
			         int32_t n)
{
	*a &= ~(1ul << n);
}

extern inline void _BITCLEAR_U64(uint64_t *a,
			         int32_t n)
{	
	*a &= ~(1ull << n);
}

#define BITCLEAR(a, n) _Generic((a), \
uint32_t : _BITCLEAR_U32, \
uint64_t : _BITCLEAR_U64) ((&a), (n))

extern inline void _BITSET_U32(uint32_t *a,
			       int32_t n)
{
	*a |= 1ul << n;
}

extern inline void _BITSET_U64(uint64_t *a,
			       int32_t n)
{
	*a |= 1ull << n;
}

#define BITSET(a, n) _Generic((a), \
uint32_t : _BITSET_U32, \
uint64_t : _BITSET_U64) ((&a), (n))

extern inline void _BITTOGGLE_U32(uint32_t *a,
			          int32_t n)
{	
	*a ^= (1 << n);
}

extern inline void _BITTOGGLE_U64(uint64_t *a,
				  int32_t n)
{	
	*a ^= (1ul << n);
}

#define BITTOGGLE(a, n) _Generic((a), \
uint32_t : _BITTOGGLE_U32, \
uint64_t : _BITTOGGLE_U64) ((&a), (n))

#endif