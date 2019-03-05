// File: tq_file.h
// Creator: Antonio Fiallos
// Description: File I/O helper functions

#pragma once
#include <stdlib.h>
#include "tq.h"
#include "tq_string.h"

TQ_API_LOCAL tq_String tq_FileRead(tq_String fileName)
{
	tq_String result = { 0 };
	FILE* fh = fopen(tq_StrCStr(fileName), "rb");
	if (!fh)
	{
		fprintf(stderr, "No file found; \n");
		return result;
	}
	fseek(fh, 0, SEEK_END);
	int count = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	result.data_p = (U8*)malloc(count);
	fread(result.data_p, sizeof(char), count, fh);
	result.count = count;

	fclose(fh);

	return result;
}