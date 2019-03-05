// File: tq_sting.h
// Creator: Antonio Fiallos
// Description: Definition of a length based string and it's utilities

#pragma once
#include "tq.h"
#include <stdlib.h>
#include <string.h>

typedef buffer tq_String;

//-----------------------------------------------------------------------------
// String API
//-----------------------------------------------------------------------------
TQ_API_LOCAL tq_String tq_StrNew(const char* c_str);
TQ_API_LOCAL char* tq_StrCStr(tq_String s);
TQ_API_LOCAL BOOL tq_StrEq(tq_String s, const char* c_str);
TQ_API_LOCAL S32 tq_StrToNum(tq_String s);
TQ_API_LOCAL BOOL tq_StrIsNum(tq_String s);
TQ_API_LOCAL tq_String tq_StrTrim(tq_String s);
TQ_API_LOCAL tq_String tq_StrAppend(tq_String s, const char* c_str);
TQ_API_LOCAL tq_String tq_StrCat(tq_String s1, tq_String s2);
TQ_API_LOCAL void tq_StrPrint(tq_String s);

//-----------------------------------------------------------------------------
// String API - Definitions
//-----------------------------------------------------------------------------
TQ_API_LOCAL tq_String tq_StrNew(const char* c_str)
{
	tq_String result;
	result.count = strlen(c_str);
	result.data_p = (U8*)malloc(result.count + 1);
	memcpy(result.data_p, c_str, result.count + 1); // copy the '\0' at the end in case we need it

	return result;
}

TQ_API_LOCAL char* tq_StrCStr(tq_String s)
{
	return (char*)s.data_p; // we keep the '/0' at the end of the string so we can return this
}

TQ_API_LOCAL BOOL tq_StrEq(tq_String s, const char* c_str)
{
	BOOL result = false;
	if (c_str)
	{
		U32 count = 0;
		while (*c_str != '\0')
		{
			if (*c_str != *((char*)s.data_p++))
			{
				return false;
			}
			count++;
			c_str++;
		}
		result = (count == s.count);
	}
	return result;
}

TQ_API_LOCAL char tq_StrGetChar(tq_String s, U32 pos)
{
	return (char)s.data_p[pos];
}

TQ_API_LOCAL S32 tq_StrToNum(tq_String s)
{
	S32 result = strtol((const char*)s.data_p, NULL, 10);
	return result;
}

TQ_API_LOCAL BOOL tq_StrIsNum(tq_String s)
{
	for (U32 i = 0; i < s.count; i++)
	{
		if (!IsNumber(tq_StrGetChar(s, i))) return false;
	}
	return true;
}

TQ_API_LOCAL tq_String tq_StrTrim(tq_String s)
{
	U32 firstNonW = 0;
	while (IsSpacing(tq_StrGetChar(s, firstNonW))) 
	{ 
		firstNonW++; 
	}

	U32 pos = firstNonW;
	U32 lastNonW = firstNonW;
	for (;pos < s.count; pos++)
	{
		if (!IsSpacing(tq_StrGetChar(s, pos)))
		{
			lastNonW = pos;
		}
	}

	tq_String result;
	result.count = (lastNonW - firstNonW) + 1;
	result.data_p = &s.data_p[firstNonW];

	return result;
}

TQ_API_LOCAL tq_String tq_StrAppend(tq_String s, const char* c_str)
{
	U32 clen = strlen(c_str);
	tq_String result;
	result.count = s.count + clen;
	result.data_p = (U8*)realloc(s.data_p, result.count);
	memcpy(&result.data_p[s.count], c_str, clen);

	return result;
}

TQ_API_LOCAL tq_String tq_StrCat(tq_String s1, tq_String s2)
{
	tq_String result;
	result.count = s1.count + s2.count;
	result.data_p = (U8*)malloc(result.count);
	memcpy(result.data_p, s1.data_p, s1.count);
	memcpy(&result.data_p[s1.count], s2.data_p, s2.count);

	return result;
}

TQ_API_LOCAL void tq_StrPrint(tq_String s)
{
	printf("!%.*s!\n", s.count, s.data_p);
}