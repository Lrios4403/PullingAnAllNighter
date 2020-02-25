#ifndef ENGINE_H
#define ENGINE_H

#include "olcConsoleGameEngine.h"

#include "anticheat.h"
#include "sound.h"
#include "steam.h"
#include "consoleManager.h"

#include "scene.h"
#include "scenehome.h"
#include "scenemain01.h"
#include "debug.h"
#include "macro.h"

#define _WIN32_WINNT 0x0500
#include <windows.h>

/*=====================================================================================================================*/
/*
/*	TODO:	+ Make a console manager class
/*				- Changes\Saves the registery keys for the:
/*					~ Color
/*					~ Font
/*
/*=====================================================================================================================*/

class Engine : public olcConsoleGameEngine
{
public:
	long sceneIndex = 0;
	std::vector<Scene*> scenes;

	bool bFistRun = false;

	Anticheat anticheat;
	SoundManager soundManager;
	////steamManager //steamManager;
	ConsoleManager consoleManager;

	DWORD dwConsoleColors[16];
	DWORD dwPrevConsoleColors[16];

	Engine()
	{
		m_sAppName = L"Pulling An All Nighter";
	}

	bool OnUserCreate() override
	{
		odprintf("Initalizing Engine...");
		debugTabIndex++;

		ShowCursor(false);

		odprintf("Initalizing basic classes...");
		debugTabIndex++;
		anticheat = Anticheat();
		anticheat.Initalize();
		anticheat.StartAnticheat();
		consoleManager = ConsoleManager();
		consoleManager.Initalize(this);
		soundManager = SoundManager();
		soundManager.Initalize();
		//steamManager = //steamManager();
		//steamManager.Initalize();
		debugTabIndex--;

		odprintf("Initalizing all the scenes...");
		scenes.push_back(new SceneTitlescreen());
		scenes[scenes.size() - 1]->Initalize(this);
		scenes.push_back(new Scene01());
		scenes[scenes.size() - 1]->Initalize(this);

		debugTabIndex--;
		odprintf("Initalzed Engine!");
		odprintf("");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		long lastIndex = sceneIndex;

		SWITCHANDRUN(bFistRun, scenes[sceneIndex]->Switch(sceneIndex);)

		scenes[sceneIndex]->Update(fElapsedTime, &sceneIndex);

		ShowWindow(GetConsoleWindow(), SW_SHOW);

		if (sceneIndex != lastIndex)
		{
			scenes[sceneIndex]->Switch(lastIndex);
			scenes[lastIndex]->End();
		}

		return true;
	}

	bool OnUserDestroy() override
	{
		odprintf("");
		odprintf("Engine[%p].OnUserDestroy()...", this);

		debugTabIndex++;

		odprintf("Cleaning the scenes...");

		for (auto s : scenes)
		{
			s->End();
		}

		odprintf("Calling class destructors...");
		//consoleManager.Destroy();

		odprintf("Exiting...");

		debugTabIndex--;

		return true;
	}

};

#endif