#ifndef ENGINE_H
#define ENGINE_H

#include "olcConsoleGameEngine.h"

#include "anticheat.h"
#include "sound.h"
#include "fonthandler.h"
#include "steam.h"
#include "consoleManager.h"

#include "scene.h"
#include "scenehome.h"
#include "scenemain01.h"
#include "scenemain11.h"
#include "scenetest001.h"
#include "scenedone.h"
#include "debug.h"
#include "macro.h"

#define WINVER			0x0400 
#define _WIN32_WINNT	0x0400 
#include <windows.h>

/*=====================================================================================================================*/
/*
/*	TODO:	+
/*
/*=====================================================================================================================*/

class Engine : public olcConsoleGameEngine
{
public:
	long lastIndex = -1;
	std::vector<Scene*> scenes;

	bool bFistRun = false;

	Anticheat anticheat;
	SoundManager soundManager;
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
		FontHandler::Initalize();
		debugTabIndex--;

		odprintf("Initalizing all the scenes...");
		scenes.push_back(new SceneTitlescreen);
		scenes.push_back(new Scene11);
		scenes.push_back(new SceneDone);
		//scenes.push_back(new SceneTest001);
		for (size_t t = 0; t < scenes.size(); t++)
			scenes[t]->Create(this);
		odprintf("Total initalization time:");
		debugTabIndex++;
		for (size_t t = 0; t < scenes.size(); t++)
			odprintf("%f", scenes[t]->fCreateTime);
		debugTabIndex--;

		debugTabIndex--;
		odprintf("Initalzed Engine!");
		odprintf("");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (sceneIndex != lastIndex)
		{
			scenes[sceneIndex]->Switch(lastIndex);
			if(scenes.size() >= lastIndex || lastIndex > 0) scenes[lastIndex]->End();
			lastIndex = sceneIndex;
		}

		scenes[sceneIndex]->Update(fElapsedTime, &sceneIndex);

		if (!bFistRun)
		{
			consoleManager.ShowConsole(true);
			consoleManager.CenterWindow();
			bFistRun = true;
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