#include <stdio.h>
#include <wtypes.h>
#include "Functions.h"

WCHAR* ReadTextFromFile(const WCHAR path[])
{
	FILE* file;
	WCHAR currString[1024];
	WCHAR result[5096] = L"                        ";

	_wfopen_s(&file, path, L"r");
	while (fgetws(currString, 1024, file) != NULL)
	{
		lstrcatW(result, currString);
	}
	for (int i = 0, len = lstrlenW(result); i < len; i++)
	{
		if (result[i] == L'\n') { result[i] = L' '; }
		if (result[i + 1] != L'\0')
		{
			if (result[i] == L' ' && result[i - 1] != L' ' && result[i + 1] != L' ')
			{
				result[i] = L'_';
			}
		}
	}
	fclose(file);

	return result;
}
void SetUiElements(UI_ELEMENTS* PUI)
{
	POINT coord = { (*PUI).windowRect.right / 2 - 250, (*PUI).windowRect.bottom / 2 + 10 };
	POINT size = { 500, 50 };

	SetRect(&(*PUI).lettersRect, coord.x, coord.y, coord.x + size.x, coord.y + size.y);
	SetRect(&(*PUI).greencounterRect, coord.x, coord.y - 75, coord.x + 100, coord.y - 25);//135, 235
	SetRect(&(*PUI).redcounterRect, coord.x + 125, coord.y - 75, coord.x + 225, coord.y - 25);//125. 225
	SetRect(&(*PUI).arrowsRect, coord.x + size.x - 60, coord.y + size.y - 100, coord.x + size.x + 15, coord.y + size.y - 50);
}
void IterateCounter(char counter[])
{
	for (int i = 4; i >= 0; i--)
	{
		if (counter[i] < '9')
		{
			counter[i]++;
			return;
		}
		else
		{
			counter[i] = '0';
		}
	}
}
COLORREF ChangeTextBrightness(const int red, const int green, const int blue, int* frame)
{
	static int brightness = 0;

	if ((*frame)++ < 10)
	{
		brightness += 20;
	}
	else
	{
		if (brightness == 0)
		{
			*frame = 0;
		}
		brightness -= 20;
	}

	return RGB(red > 0 ? brightness + red : 0, green > 0 ? brightness + green : 0, blue > 0 ? brightness + blue : 0);
}