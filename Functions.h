#pragma once
typedef enum { START, MAIN, WIN, LOSE } GAME_STATE;

typedef struct
{
	RECT windowRect;
	RECT lettersRect;
	RECT greencounterRect;
	RECT redcounterRect;
	RECT arrowsRect;
} UI_ELEMENTS;

WCHAR* ReadTextFromFile(const WCHAR path[]);
void  SetUiElements(UI_ELEMENTS* PUI);
void IterateCounter(char counter[]);
COLORREF ChangeTextBrightness(const int red, const int green, const int blue, int* frame);