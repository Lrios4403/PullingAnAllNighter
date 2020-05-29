#ifndef CONSOLEMANANGER_H
#define CONSOLEMANANGER_H

#include "olcConsoleGameEngine.h"

#include "anticheat.h"
#include "sound.h"
#include "steam.h"
#include "regedit.h"

#include "scene.h"
#include "scenehome.h"
#include "scenemain01.h""
#include "debug.h"
#include "macro.h"

#include <windows.h>

//Needed for easier reading
void* pConsoleManager = nullptr;
#define ConsoleMacro ((ConsoleManager*)pConsoleManager)

DWORD defColorPalette[16] =
{
		789516,
		14300928,
		958739,
		14521914,
		2035653,
		9967496,
		40129,
		13421772,
		7763574,
		16742459,
		837142,
		14079585,
		5654759,
		10354868,
		10875385,
		15921906
};

class ConsoleManager
{
private:
	
	CONSOLE_SCREEN_BUFFER_INFOEX	infoCurrent;
	CONSOLE_FONT_INFOEX				infoFont;
	olcConsoleGameEngine*			olce;

public:

	int Initalize(olcConsoleGameEngine* olc)
	{
		odprintf("Initalizing ConsoleManager[%p]...", this);
		debugTabIndex++;

		olce = olc;

		SetPalette(defColorPalette);

		pConsoleManager = this;

		debugTabIndex--;
		return 0;
	}

	int SetPalette(DWORD* colorPallete)
	{
		infoCurrent.cbSize = sizeof(infoCurrent);
		HANDLE	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfoEx(hConsole, &infoCurrent);
		for (int i = 0; i < 16; i++)infoCurrent.ColorTable[i] = colorPallete[i];
		SetConsoleScreenBufferInfoEx(hConsole, &infoCurrent);

		return 0;

	}

	int ShowConsole(bool bShow)
	{
		if(bShow)
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		else
			ShowWindow(GetConsoleWindow(), SW_HIDE);

		return 0;
	}

	bool CenterWindow()
	{
		HWND ConsoleWindow;
		ConsoleWindow = GetForegroundWindow();

		//Getting the desktop hadle and rectangule
		HWND   hwndScreen;
		RECT   rectScreen;
		hwndScreen = GetDesktopWindow();
		GetWindowRect(hwndScreen, &rectScreen);

		RECT rConsole;
		GetWindowRect(GetConsoleWindow(), &rConsole);
		int Width = rConsole.left = rConsole.right;
		int Height = rConsole.bottom - rConsole.top;

		//caculate the window console to center of the screen	
		int ConsolePosX;
		int ConsolePosY;
		ConsolePosX = ((rectScreen.right - rectScreen.left) / 2 - Width / 2);
		ConsolePosY = ((rectScreen.bottom - rectScreen.top) / 2 - Height / 2);
		SetWindowPos(GetConsoleWindow(), NULL, ConsolePosX, ConsolePosY, Width, Height, SWP_SHOWWINDOW || SWP_NOSIZE);

		SwitchToThisWindow(GetConsoleWindow(), true);

		int width = rectScreen.right - rectScreen.left;
		int height = rectScreen.bottom - rectScreen.top;
		POINT cursorPos;
		SetCursorPos((width / 2) + rectScreen.left, (height / 2) + rectScreen.top);

		return true;
	}


	int Destroy()
	{
		odprintf("ConsoleManager[%p].Destroy()...", this);
		debugTabIndex++;

		debugTabIndex--;
		return 0;
	}

};

#endif