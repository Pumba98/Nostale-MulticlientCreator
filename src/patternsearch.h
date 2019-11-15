#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <cstdio>

class PTSearch
{
public:
    static DWORD FindPattern(FILE* inFile, const char *pattern, const char *mask)
	{
		DWORD patternLength = (DWORD)strlen(mask);
		long base = 0x249F00;
		int i = 0;

		char currend;
		while (!feof(inFile) && !ferror(inFile))
		{
			bool found = true;
			fseek(inFile, base + i, SEEK_SET);
			for (DWORD j = 0; j < patternLength; j++)
			{
				fread(&currend, 1, 1, inFile);
				found &= mask[j] == '?' || pattern[j] == currend;
			}

			if (found)
			{
				return base + i;
			}
			i++;
		}
		return -1;
	}
};
