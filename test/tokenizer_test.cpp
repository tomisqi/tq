#include <stdio.h>
#include "tq.h"
#include "tq_file.h"
#define TQ_TOKENIZER_IMPLEMENTATION
#include "tq_tokenizer.h"

#define MAX_SISESSIONS 10

struct SiSession
{
	U32 period;
	U32 pduSize;
	U32 rep;
	U32 siId;
};

static SiSession ParseSiSession(tq_Tokenizer* tokenizer_p)
{
	SiSession result = {};
	bool parsing = true;
	while (parsing)
	{
		tq_Token token = tq_TokGet(tokenizer_p);
		switch (token.type)
		{
		case TOKEN_ENDOFSTREAM:
			parsing = false;
			break;
		case TOKEN_KEY:
		{
			tq_Token tokenVal = tq_TokRequire(tokenizer_p, TOKEN_VALUE);
			S32 value = tq_TokGetNumber(tokenizer_p, tokenVal);
			if (tq_TokEquals(token, "period"))	result.period = value;
			if (tq_TokEquals(token, "pduSize"))	result.pduSize = value;
			if (tq_TokEquals(token, "rep"))		result.rep = value;
			if (tq_TokEquals(token, "siId"))	result.siId = value;
		}
		break;
		default:
			break;
		}
	}

	return result;
}

int main(int argc, char** argv)
{
	tq_String fileName = tq_StrNew("Data/Example0.yaml");
	tq_String fileContents = tq_FileRead(fileName);
	if (fileContents.count == 0)
	{
		return 1;
	}

	tq_Tokenizer tokenizer = tq_Tokenize(fileName, fileContents);
	SiSession siSessions[MAX_SISESSIONS] = {};
	U32 sessionCount = 0;

	bool parsing = true;
	while (parsing)
	{
		tq_Token token = tq_TokGet(&tokenizer);
		switch (token.type)
		{
		case TOKEN_ENDOFSTREAM:
			parsing = false;
			break;
		case TOKEN_LISTMEMBER:
		{
			if (tq_TokEquals(token, "siSession"))
			{
				tq_Tokenizer sessionTokenizer = tq_TokenizeSameIndent(&tokenizer);
				//PrintString(sessionTokenizer.input);
				siSessions[sessionCount] = ParseSiSession(&sessionTokenizer);
				sessionCount++;
				TQ_ASSERT(sessionCount < MAX_SISESSIONS);
			}
		}
		break;
		default:
			break;
		}
	}

	printf("siSessions:\n");
	for (U32 i = 0; i < sessionCount; i++)
	{
		printf("---\n");
		SiSession* si_p = &siSessions[i];
		printf("siId=%d\nperiod=%u\npduSize=%u\nrep=%d\n",
				si_p->siId,
				si_p->period,
				si_p->pduSize,
				si_p->rep);
	}

	return 0;
}