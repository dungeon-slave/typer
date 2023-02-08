#include <Windows.h>
#include <wchar.h>
#include "Functions.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static GAME_STATE  typerstate = START;
	static UI_ELEMENTS UI;

	static HDC		   hdc;
	static UINT_PTR    timer = 1;
	static PAINTSTRUCT ps;

	static WCHAR*	   wstring;
	static WCHAR*	   copywstring;
	static HFONT       hFont;

	static HBRUSH      windowbrush;
	static HBRUSH      lettersrectbrush;

	static int         time = 400;
	static int         combocounter = 0;
	static char        redcounter[] = "00000";
	static char        greencounter[] = "00000";

	static int         countersbkstate = 0;
	static int         lettersbkstate = 0;
	static int         frame = 0;

	switch (message)
	{
		case WM_CREATE:
		{
			hdc = GetDC(hWnd);

			GetClientRect(hWnd, &UI.windowRect);
			SetUiElements(&UI);
			SetTimer(hWnd, timer, time, NULL);

			hFont = CreateFont(50, 20, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));

			windowbrush = CreateSolidBrush(RGB(0, 0, 0));
			lettersrectbrush = CreateSolidBrush(RGB(100, 100, 100));

			break;
		}
		case WM_DROPFILES:
		{
			if (typerstate == START)
			{
				WCHAR* wfilepath = malloc(sizeof(WCHAR) * 512);
				wstring = malloc(sizeof(WCHAR) * 5096);
				copywstring = malloc(sizeof(WCHAR) * 5096);
				DragQueryFileW((HDROP)wParam, 0, wfilepath, (int)wcslen(wfilepath));
				lstrcpynW(wstring, ReadTextFromFile(wfilepath), 5096);
				lstrcpynW(copywstring, wstring, (int)wcslen(wstring));
				free(wfilepath);
			}
			break;
		}
		case WM_TIMER:
		{
			switch (typerstate)
			{
				case MAIN:
				{
					switch (wParam)
					{
						case 1:
						{
							static int speed = 0;

							copywstring++;
							speed++;
							if (copywstring[0] != L' ')
							{
								speed = 0;
								typerstate = LOSE;
							}
							if (combocounter > 50)
							{
								time += 25;
								KillTimer(hWnd, timer);
								SetTimer(hWnd, timer, time, NULL);
								combocounter = 0;
								lettersbkstate = 1;
							}
							if (speed > 40)
							{
								time -= speed / 2;
								KillTimer(hWnd, timer);
								SetTimer(hWnd, timer, time, NULL);
								speed = 0;
								lettersbkstate = 2;
							}
							break;
						}
					}
					break;
				}
				default:
				{
					if (time != 400) 
					{
						time = 400;
						KillTimer(hWnd, timer);
						SetTimer(hWnd, timer, time, NULL);
					}
					break;
				}
			}
			InvalidateRect(hWnd, &UI.windowRect, 0);
			break;
		}
		case WM_SIZE:
		{
			GetClientRect(hWnd, &UI.windowRect);
			SetUiElements(&UI);

			break;
		}
		case WM_CHAR:
		{
			switch (typerstate)
			{
				case MAIN:
				{
					if (timer)
					{
						int pos = 0;

						while (copywstring[pos] == L' ') { pos++; }
						if (wParam == copywstring[pos] || (wParam == L' ' && copywstring[pos] == L'_'))
						{
							copywstring[pos] = L' ';
							combocounter++;
							IterateCounter(greencounter);
							if (copywstring[pos + 1] == L'\0')
							{
								typerstate = WIN;
							}
							countersbkstate = 1;
						}
						else
						{
							IterateCounter(redcounter);
							combocounter = 0;
							countersbkstate = 2;
						}
					}
					InvalidateRect(hWnd, NULL, 0);
					break;
				}
				default:
				{
					if (wstring)
					{
						switch (wParam)
						{
							case 13:
							{
								lstrcpynW(copywstring, wstring, (int)wcslen(wstring));
								break;
							}
							case 27:
							{
								free(wstring);
								wstring = copywstring = NULL;
								break;
							}
						}
						strcpy_s(greencounter, 6, "00000");
						strcpy_s(redcounter, 6, "00000");

						typerstate = !typerstate && 1;
					}
					break;
				}
			}
			break;
		}
		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			FillRect(hdc, &UI.windowRect, windowbrush);			
			SelectObject(hdc, hFont);

			SetTextColor(hdc, RGB(0, countersbkstate == 1 ? 150 : 0, 0));
			SetBkColor(hdc, RGB(0, countersbkstate == 1 ? 100 : 50, 0));
			DrawTextA(hdc, greencounter, 5, &UI.greencounterRect, DT_CENTER);

			SetTextColor(hdc, RGB(countersbkstate == 2 ? 150 : 0, 0, 0));
			SetBkColor(hdc, RGB(countersbkstate == 2 ? 100 : 50, 0, 0));
			DrawTextA(hdc, redcounter, 5, &UI.redcounterRect, DT_CENTER);

			countersbkstate = 0;

			switch (typerstate)
			{
				case START:
				{
					WCHAR hellostr[64];
					if (!copywstring)
					{
						const WCHAR str[] = L"DRAG AND DROP FILE HERE!\0";
						if (lstrcmpW(hellostr, str))
						{
							lstrcpynW(hellostr, str, 64);
						}
					}
					else
					{
						const WCHAR str[] = L"PRESS ENTER TO START!\0";
						if (lstrcmpW(hellostr, str))
						{
							lstrcpynW(hellostr, str, 64);
						}
					}
					SetTextColor(hdc, ChangeTextBrightness(50, 50, 50, &frame));
					SetBkColor(hdc, RGB(0, 0, 0));
					DrawTextW(hdc, hellostr, (int)wcslen(hellostr), &UI.lettersRect, DT_CENTER);

					break;
				}
				case MAIN:
				{
					switch (lettersbkstate)
					{
						case 0:
						{
							SetTextColor(hdc, RGB(125, 125, 125));
							break;
						}
						case 1://Замедление
						{
							if (frame > 3)
							{
								frame = 0;
								lettersbkstate = 0;
							}
							SetTextColor(hdc, RGB(115, 241, 255));
							SetBkColor(hdc, RGB(0, 0, 0));
							DrawTextW(hdc, L" <<", 3, &UI.arrowsRect, DT_LEFT);
							frame++;

							break;
						}
						case 2://Ускорение
						{
							if (frame > 3)
							{
								frame = 0;
								lettersbkstate = 0;
							}
							SetTextColor(hdc, RGB(255, 254, 145));
							SetBkColor(hdc, RGB(0, 0, 0));
							DrawTextW(hdc, L">>>", 3, &UI.arrowsRect, DT_LEFT);
							frame++;

							break;
						}
					}
					SetBkColor(hdc, RGB(0, 0, 0));
					DrawTextW(hdc, copywstring, (int)wcslen(copywstring), &UI.lettersRect, DT_LEFT);

					break;
				}
				case WIN:
				{
					const WCHAR hellostr[] = L"ENTER=RESTART ESC=MENU";
					SetTextColor(hdc, ChangeTextBrightness(0, 50, 0, &frame));
					SetBkColor(hdc, RGB(0, 0, 0));
					DrawTextW(hdc, hellostr, (int)wcslen(hellostr), &UI.lettersRect, DT_CENTER);

					break;
				}
				case LOSE:
				{
					const WCHAR hellostr[] = L"ENTER=RESTART ESC=MENU";
					SetTextColor(hdc, ChangeTextBrightness(50, 0, 0, &frame));
					SetBkColor(hdc, RGB(0, 0, 0));
					DrawTextW(hdc, hellostr, (int)wcslen(hellostr), &UI.lettersRect, DT_CENTER);

					break;
				}
			}

			FrameRect(hdc, &UI.lettersRect, lettersrectbrush);

			EndPaint(hWnd, &ps);

			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hWnd, timer);
			DeleteObject(hFont);
			free(wstring);
			PostQuitMessage(0);

			break;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}