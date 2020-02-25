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

void* pConsoleManager = nullptr;

enum ConsoleError
{
	FailedToGetRegisteryKey = 1,
	FailedToGetRegisteryValue,
};

struct ConsoleColorPalette
{
	DWORD dwColors[16];
};

ConsoleColorPalette defColorPalette =
{
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
		15921906,
	}
};

class ConsoleManager
{
private:
	
	HKEY hKey;

	ConsoleColorPalette befColorPalette;

	vector<wstring> wKeyNames =
	{
		L"ColorTable00",
		L"ColorTable01",
		L"ColorTable02",
		L"ColorTable03",
		L"ColorTable04",
		L"ColorTable05",
		L"ColorTable06",
		L"ColorTable07",
		L"ColorTable08",
		L"ColorTable09",
		L"ColorTable10",
		L"ColorTable11",
		L"ColorTable12",
		L"ColorTable13",
		L"ColorTable14",
		L"ColorTable15",
	};


public:

	int Initalize(olcConsoleGameEngine* olc)
	{
		odprintf("Initalizing ConsoleManager[%p]...", this);

		debugTabIndex++;

		int iReturn = 0;

		//Get the registery console info
		odprintf("Getting the console HKEY...");
		iReturn = RegOpenKeyEx( HKEY_CURRENT_USER,L"Console", NULL, KEY_QUERY_VALUE, &hKey);

		if (iReturn != ERROR_SUCCESS)
		{
			odprintf("ERROR: Failed to use RegOpenKeyEx. [%i] !", iReturn);
			debugTabIndex--;
			return FailedToGetRegisteryKey;
		}

		//Collect the default color pallete to set back when the console is destroyed
		odprintf("Reading\\Setting the console table registery keys...");

		/*
		for (int i = 0; i < wKeyNames.size(); i++)
		{
			odprintf("%ls", wKeyNames[i].c_str());
			befColorPalette.dwColors[i] = GetValueFromRegistry(hKey, wKeyNames[i].c_str());
			SetRegistryValues(hKey, wKeyNames[i].c_str(), defColorPalette.dwColors[i]);
		}
		*/

		befColorPalette = GetPalette();
		SetPalette(&defColorPalette);

		debugTabIndex--;
		
		odprintf("Initalized ConsoleManager[%p]!", this);

		return 0;
	}

	int SetPalette(ConsoleColorPalette* ccp)
	{
		odprintf("ConsoleManager[%p].SetPalette(%p)...", this, ccp);

		debugTabIndex++;

		for (int i = 0; i < wKeyNames.size(); i++)
		{
			odprintf("%ls | 0x0%08x", wKeyNames[i].c_str(), ccp->dwColors[i]);
			SetRegistryValues(hKey, wKeyNames[i].c_str(), ccp->dwColors[i]);
		}

		debugTabIndex--;

		return 0;

	}

	ConsoleColorPalette GetPalette()
	{
		odprintf("ConsoleManager[%p].GetPalette()...", this);
		debugTabIndex++;

		ConsoleColorPalette retp;

		for (int i = 0; i < wKeyNames.size(); i++)
		{
			odprintf("%ls", wKeyNames[i].c_str());
			retp.dwColors[i] = GetValueFromRegistry(hKey, wKeyNames[i].c_str());
		}

		debugTabIndex--;

		return retp;

	}

	int Destroy()
	{
		odprintf("ConsoleManager[%p].Destroy()...", this);

		debugTabIndex++;

		odprintf("Returning the color palette to its origional schema...");
		SetPalette(&befColorPalette);

		debugTabIndex--;

		return 0;
	}

};

//Needed for easier reading
#define ConsoleMacro ((ConsoleManager*)pConsoleManager)

#endif