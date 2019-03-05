// File: tq_tokenizer.h
// Creator: Antonio Fiallos
// Description: Simple tokenizer for yaml files with *many,many* limitations

// In *one* of your c/cpp files, do
//   #define TQ_TOKENIZER_IMPLEMENTATION
// before this file is included

// Known limitations:
//   Only supports YAML types: list members, dictionaries {key: value}, comments
//   Relaxed check for spacing compared to requirements in YAML format
//   No support for:
//     Dictionaries in abbreviated form
//     Boolean values
//     Multi-lines
//     Brackets for lists
//     Exclamation for node labeling

#pragma once
#include "tq.h"
#include "tq_string.h"

enum tq_TokenType
{
	TOKEN_UNKNOWN,
	TOKEN_NULL,

	TOKEN_LISTMEMBER,
	TOKEN_KEY,
	TOKEN_VALUE,
	TOKEN_COMMENT,

	TOKEN_ENDOFSTREAM,
};

struct tq_Token
{
	tq_TokenType type;
	tq_String text;
};

struct tq_Tokenizer
{
	tq_String stream;
	U32 pos;

	tq_String fileName;
	U32 lineNo;
	U32 indentation;
};

//-----------------------------------------------------------------------------
// Tokenizer API
//-----------------------------------------------------------------------------
TQ_API tq_Tokenizer tq_Tokenize(tq_String fileName, tq_String stream, U32 lineStart);
TQ_API void tq_TokPrint(tq_Token token);
TQ_API bool tq_TokEquals(tq_Token token, const char* match);
TQ_API tq_Token tq_TokGet(tq_Tokenizer* tokenizer_p);
TQ_API tq_Token tq_TokRequire(tq_Tokenizer* tokenizer_p, tq_TokenType type);
TQ_API S32 tq_TokGetNumber(tq_Tokenizer* tokenizer_p, tq_Token token);
TQ_API tq_Tokenizer tq_TokenizeSameIndent(tq_Tokenizer* tokenizer_p);

#ifdef TQ_TOKENIZER_IMPLEMENTATION

//-----------------------------------------------------------------------------
// Internal
//-----------------------------------------------------------------------------
internal void tq__AdvanceChars(tq_Tokenizer* tokenizer_p, U32 amount)
{
	tokenizer_p->pos += amount;
}

internal char tq__GetChar(tq_Tokenizer* tokenizer_p)
{
	U32 pos = tokenizer_p->pos;
	return (tq_StrGetChar(tokenizer_p->stream, pos));
}

internal void tq__SkipWhiteSpace(tq_Tokenizer* tokenizer_p)
{
	tokenizer_p->indentation = 0;
	while (IsWhiteSpace(tq__GetChar(tokenizer_p)))
	{
		if (tq__GetChar(tokenizer_p) == '\n')
		{
			tokenizer_p->lineNo++;
			tokenizer_p->indentation = 0;
		}
		else
		{
			tokenizer_p->indentation++;
		}
		tq__AdvanceChars(tokenizer_p, 1);
	}
}

internal void tq__AdvanceToColonOrLineEnd(tq_Tokenizer* tokenizer_p)
{
	char c = tq__GetChar(tokenizer_p);
	while (!IsEndOfLine(c) &&
		   !(c == ':'))
	{
		tq__AdvanceChars(tokenizer_p, 1);
		c = tq__GetChar(tokenizer_p);
	}
}

internal BOOL tq__AdvanceToColon(tq_Tokenizer* tokenizer_p)
{
	tq__AdvanceToColonOrLineEnd(tokenizer_p);
	return tq__GetChar(tokenizer_p) == ':';
}

internal BOOL tq__IsEndOfStream(tq_Tokenizer* tokenizer_p)
{
	return tokenizer_p->pos >= (tokenizer_p->stream.count - 1);
}

internal void tq__AdvanceToLineEnd(tq_Tokenizer* tokenizer_p)
{
	while (!IsEndOfLine(tq__GetChar(tokenizer_p)) &&
		   !tq__IsEndOfStream(tokenizer_p))
	{
		tq__AdvanceChars(tokenizer_p, 1);
	}
}

internal void tq__NextLine(tq_Tokenizer* tokenizer_p)
{
	while (tq__GetChar(tokenizer_p) != '\n' &&
		!tq__IsEndOfStream(tokenizer_p))
	{
		tq__AdvanceChars(tokenizer_p, 1);
	}
	tq__AdvanceChars(tokenizer_p, 1);
	tokenizer_p->lineNo++;
}

internal char* tq__ErrorHeader(tq_Tokenizer* tokenizer_p)
{
	static char hdr[64] = {};
	sprintf(hdr, "%.*s (lineNo=%u)",
		tokenizer_p->fileName.count,
		tokenizer_p->fileName.data_p,
		tokenizer_p->lineNo);
	return hdr;
}


//-----------------------------------------------------------------------------
// Tokenizer API - Definitions
//-----------------------------------------------------------------------------
tq_Tokenizer tq_Tokenize(tq_String fileName, tq_String stream, U32 lineStart = 1)
{	
	tq_Tokenizer result = {};
	result.fileName = fileName;
	result.stream = stream;
	result.pos = 0;
	result.lineNo = lineStart;
	return result;
}

tq_Tokenizer tq_TokenizeSameIndent(tq_Tokenizer* tokenizer_p)
{
	U32 startIndent = tokenizer_p->indentation;
	U32 startPos = tokenizer_p->pos;
	U32 startLineNo = tokenizer_p->lineNo;
	do
	{
		tq__NextLine(tokenizer_p);
		tq__SkipWhiteSpace(tokenizer_p);
	} while (tokenizer_p->indentation > startIndent);

	tq_String stream;
	stream.count = tokenizer_p->pos - startPos;
	stream.data_p = &tokenizer_p->stream.data_p[startPos];
	tq_Tokenizer subTokenizer = tq_Tokenize(tokenizer_p->fileName, stream, startLineNo);
	return subTokenizer;
}

void tq_TokPrint(tq_Token token)
{
	printf("{type=%d text=%.*s}\n", token.type, token.text.count, token.text.data_p);
}

bool tq_TokEquals(tq_Token token, const char* match)
{
	return tq_StrEq(token.text, match);
}

tq_Token tq_TokGet(tq_Tokenizer* tokenizer_p)
{
	tq_Token token = {};
	if (tq__IsEndOfStream(tokenizer_p))
	{
		token.type = TOKEN_ENDOFSTREAM;
		return token;
	}

	tq__SkipWhiteSpace(tokenizer_p);

	char c = tq__GetChar(tokenizer_p);
	switch (c)
	{
	case '#':
	{
		tq__AdvanceChars(tokenizer_p, 1);
		U32 startPos = tokenizer_p->pos;

		tq__AdvanceToLineEnd(tokenizer_p);

		tq_String text;
		text.data_p = &tokenizer_p->stream.data_p[startPos];
		text.count = tokenizer_p->pos - startPos;

		token.type = TOKEN_COMMENT;
		token.text = text;
	}
	break;
	case '-':
	{
		tq__AdvanceChars(tokenizer_p, 1);
		U32 startPos = tokenizer_p->pos;

		tq__AdvanceToColonOrLineEnd(tokenizer_p);

		tq_String text;
		text.data_p = &tokenizer_p->stream.data_p[startPos];
		text.count = tokenizer_p->pos - startPos;
		text = tq_StrTrim(text);

		token.type = TOKEN_LISTMEMBER;
		token.text = text;
		tq__AdvanceChars(tokenizer_p, 1);
	}
	break;
	case ':':
	{
		tq__AdvanceChars(tokenizer_p, 1);
		token.type = TOKEN_NULL;
		if (!IsEndOfLine(tq__GetChar(tokenizer_p)))
		{
			U32 startPos = tokenizer_p->pos;
			tq__AdvanceToLineEnd(tokenizer_p);

			tq_String text;
			text.data_p = &tokenizer_p->stream.data_p[startPos];
			text.count = tokenizer_p->pos - startPos;
			text = tq_StrTrim(text);

			token.type = TOKEN_VALUE;
			token.text = text;
		}
	}
	break;
	default:
	{
		if (IsAlpha(c))
		{
			U32 startPos = tokenizer_p->pos;
			if (tq__AdvanceToColon(tokenizer_p))
			{
				tq_String text;
				text.data_p = &tokenizer_p->stream.data_p[startPos];
				text.count = tokenizer_p->pos - startPos;
				text = tq_StrTrim(text);

				token.text = text;
				token.type = TOKEN_KEY;
			}
			else
			{
				token.type = TOKEN_UNKNOWN;
			}
		}
		else
		{
			token.type = TOKEN_UNKNOWN;
			tq__AdvanceChars(tokenizer_p, 1);
		}
	}
	break;
	};

	//printf("%d", tokenizer_p->lineNo); tq_TokPrint(token);
	return token;
}
tq_Token tq_TokRequire(tq_Tokenizer* tokenizer_p, tq_TokenType type)
{
	tq_Token token = tq_TokGet(tokenizer_p);
	if (token.type != type)
	{
		char* errHdr = tq__ErrorHeader(tokenizer_p);
		fprintf(stderr, "%s: Unexpected token\n", errHdr);
	}
	return token;
}

S32 tq_TokGetNumber(tq_Tokenizer* tokenizer_p, tq_Token token)
{
	if (!tq_StrIsNum(token.text))
	{
		char* errHdr = tq__ErrorHeader(tokenizer_p);
		fprintf(stderr, "%s: Expected number value\n", errHdr);
		return 0;
	}
	return tq_StrToNum(token.text);
}

#endif //  TQ_TOKENIZER_IMPLEMENTATION