#ifndef SCENEDONE_H
#define SCENEDONE_H

#include "scene.h"
#include "sound.h"

class SceneDone : public Scene
{
public:
	WaveData* wavMain;
	bool bShow		= false;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[titlescreen]...");
		debugTabIndex++;

		this->olcEngine = e;

		wavMain		= SoundMacro->LoadSoundFileGetWave(L"resources\\wavEnding.wav", false, true, false);

		debugTabIndex--;
		odprintf("Initalized SCENE[titlescreen]!");
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(300/2/2, 100/2/2, 4*2*2, 8*2*2);

		wavMain->lpSecondaryBuffer->Play(0,0,DSBPLAY_LOOPING);

		return true;
	}


	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		olcEngine->Fill(0, 0, olcEngine->ScreenWidth(), olcEngine->ScreenHeight(), PIXEL_HALF, BG_BLUE | FG_BLUE);

		bShow = !bShow;
		if (bShow)
		{
			olcEngine->DrawString((300 / 2 / 2 / 2) - 12, 1 + 3, L"Congratulations! You Win!", BG_BLUE | FG_BLACK);
			olcEngine->DrawString((300 / 2 / 2 / 2) - (43 / 2), 10 + 5, L"You have been released, feel free to leave.", BG_BLUE | FG_BLACK);
		}



		Sleep(250);

		return true;
	}

	bool End() override
	{


		return true;
	}
};

#endif