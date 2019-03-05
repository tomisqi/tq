// File: tq.h
// Creator: Antonio Fiallos
// Description: Common definitions used in tq libs

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define TQ_ASSERT(_EXPR) assert(_EXPR)

#ifndef TQ_API
#define TQ_API extern
#endif

#ifndef internal
#define internal static
#endif

#ifndef TQ_API_LOCAL
#define TQ_API_LOCAL static
#endif

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8 S8;
typedef int16 S16;
typedef int32 S32;
typedef int64 S64;
typedef bool32 BOOL;

typedef uint8 U8;
typedef uint16 U16;
typedef uint32 U32;
typedef uint64 U64;

struct buffer
{
	U8* data_p;
	U32 count;
};

internal BOOL IsEndOfLine(char c)
{
	BOOL result = ((c == '\n') || (c == '\r'));
	return result;
}

internal BOOL IsSpacing(char c)
{
	BOOL result = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f'));
	return result;
}

internal BOOL IsWhiteSpace(char c)
{
	return IsSpacing(c) || IsEndOfLine(c);
}

internal BOOL IsNumber(char c)
{
	BOOL result = c >= '0' && c <= '9';
	return result;
}

internal BOOL IsAlpha(char c)
{
	BOOL result =
		(((c >= 'a') && (c <= 'z')) ||
		((c >= 'A') && (c <= 'Z')));
	return result;
}