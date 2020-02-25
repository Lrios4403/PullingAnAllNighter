#ifndef SCENETITLESCREEN_H
#define SCENETITLESCREEN_H

#include "scene.h"

#include "sound.h"

class SceneTitlescreen : public Scene
{
private:
	olcSprite* splashSprite;
	olcSprite* splashSpriteToptext;
	olcSprite* splashSpriteDesk;
	olcSprite* splashSpriteMonitor;

	olcSprite* spriteBtnPlay;
	olcSprite* spriteBtnSettings;

	olcSprite* splashSpriteToCorrupt;

	std::chrono::time_point<std::chrono::system_clock> tp1, tp2;

	POINT pMouse, pLastMouse;

	bool bEnding = false;
	bool bEndingCorrupted = false;

	int iIntroSoundIndex = 0;
	bool bSoundRunning = false;

	short lEndingLeft = 0xffff;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[titlescreen]...");

		if (SoundMacro->LoadSoundFile(L"intro.wav", true, true, true) == 0)
		{
			iIntroSoundIndex = SoundMacro->vSounds.size() - 1;
		}

		debugTabIndex++;

		olcEngine = e;

		splashSprite			= new olcSprite(L"resources\\titlescreen background.png.spr");
		splashSpriteToptext		= new olcSprite(L"resources\\titlescreen toptext.png.spr");
		splashSpriteDesk		= new olcSprite(L"resources\\titlescreen desk.png.spr");
		splashSpriteMonitor		= new olcSprite(L"resources\\titlescreen monitor.png.spr");

		spriteBtnPlay			= new olcSprite(L"resources\\btn play.png.spr");
		spriteBtnSettings		= new olcSprite(L"resources\\btn settings.png.spr");

		debugTabIndex--;
		odprintf("Initalized SCENE[titlescreen]!");


		return true;
	}

	bool Switch(long id) override
	{
		return true;
	}

	bool CorruptSound()
	{
		// Copy the wave data into the buffer.
		unsigned char* bufferPtr;																		//The pointer to the DirectSounds buffers
		unsigned long bufferSize;
		unsigned long playPos = 0;
		unsigned long writePos = 0;

		SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Stop();
		SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->GetCurrentPosition(&playPos, &writePos);

		odprintf("Copying to the secondary sound buffer");
		int iResult = SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Lock(0, SoundMacro->vSounds[iIntroSoundIndex]->header.dataSize, (void**)& bufferPtr, (DWORD*)& bufferSize, NULL, 0, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to lock the secondary buffer!");									//Print the error and return
			return false;
		}

		unsigned char maxSize = 0;
		unsigned long maxLoop = bufferSize - playPos;
		int mulu = 1;
		for (long l = 0; l < maxLoop; l++)
		{
			bufferPtr[l + playPos] = rand() % (maxSize+1);


			if (rand() % 8 == 0) maxSize++;
			if (rand() % 2 == 0) maxSize--;
			if (rand() % 6 == 0) maxSize+=3;
/*
			if (corruptionExperience == 2)
			if (maxSize > 16) maxSize = 0;

			if (corruptionExperience == 3)
			if (maxSize > 8) maxSize = 0;
*/
		}

		iResult = SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if (FAILED(iResult))
		{
			odprintf("ERROR: Failed to unlock the secondary buffer!");									//Print the error and return
			return false;
		}

		splashSpriteToCorrupt = new olcSprite(olcEngine->ScreenWidth(), olcEngine->ScreenHeight());

		SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Play(0,0,0);

	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		if (!bSoundRunning)
		{
			SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			Sleep(10);
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			bSoundRunning = true;
		}

		//Get the mouse position
		RECT rWindow;
		GetCursorPos(&pMouse);
		GetWindowRect(GetConsoleWindow(), &rWindow);

		pLastMouse = pMouse;

		pMouse.x -= rWindow.left + 8;
		pMouse.y -= rWindow.top + 30;

		pMouse.x = round(pMouse.x / 4);
		pMouse.y = round(pMouse.y / 8);

		if (pMouse.x < 0) pMouse.x = 0;												//Set into bounds
		if (pMouse.y < 0) pMouse.y = 0;
		if (pMouse.x > rWindow.right - rWindow.left) pMouse.x = rWindow.right - rWindow.left;
		if (pMouse.y > rWindow.bottom - rWindow.top) pMouse.y = rWindow.bottom - rWindow.top;

		float xoffset = ((pMouse.x - (olcEngine->ScreenWidth()/2)) / 32);
		float yoffset = ((pMouse.y - (olcEngine->ScreenHeight()/2)) / 16);
	
		if (bEnding) xoffset = yoffset = 0.0f;
		
		//Put a random pixel on the screen for cool effet
		if(!bEnding)
		for (int i = 0; i < 4; i++)
		{
			int px = (rand() % 27 - 1) + 1;
			int py = (rand() % 32 - 1) + 2;

			splashSpriteMonitor->SetColour(px, py, rand() % 0x00ff);

			px = (rand() % 27 - 1) + 1;
			py = (rand() % 32 - 1) + 2;

			splashSpriteMonitor->SetGlyph(px, py, rand() % 0xffff);

		}

		olcEngine->DrawSprite(-25 - xoffset, 0, splashSprite);
		olcEngine->DrawSprite(10 + (xoffset*2), 6 + (yoffset * 2), splashSpriteToptext);
		olcEngine->DrawSprite((xoffset / 4) - 1, (yoffset / 3) + 4 + olcEngine->ScreenHeight() - splashSpriteDesk->nHeight, splashSpriteDesk);
		olcEngine->DrawSprite((xoffset / 2.75) - 1, (yoffset/2) + olcEngine->ScreenHeight() - splashSpriteDesk->nHeight - (splashSpriteMonitor->nHeight/2)-5, splashSpriteMonitor);

		if (pMouse.x >= 51 && pMouse.x <= 51 + spriteBtnPlay->nWidth && pMouse.y >= 51 && pMouse.y <= 51 + spriteBtnPlay->nHeight)
		{
			olcEngine->DrawSprite(51, 52, spriteBtnPlay);

			if (olcEngine->GetMouse(0).bHeld)
			{
				bEnding = true;
			}
		}
		else
			olcEngine->DrawSprite(51, 51, spriteBtnPlay);

		if (pMouse.x >= 51 && pMouse.x <= 51 + spriteBtnSettings->nWidth && pMouse.y >= 51+16 && pMouse.y <= 51 + 16+ spriteBtnSettings->nHeight)
			olcEngine->DrawSprite(51, 52+16, spriteBtnSettings);
		else
			olcEngine->DrawSprite(51, 51 + 16, spriteBtnSettings);

		if (bEnding)
		{
			if (!bEndingCorrupted)
			{
				CorruptSound();
				bEndingCorrupted = true;
			}

			for (int x = 0; x < splashSprite->nWidth; x++) for (int y = 0; y < splashSprite->nHeight; y++)
			{
				short s = splashSprite->GetColour(x,y);

				if(s > 0) s--;

				splashSprite->SetColour(x, y, s);
				splashSprite->SetGlyph(x, y, PIXEL_HALF);
				
			}
			
			for (int x = 0; x < splashSpriteToptext->nWidth; x++) for (int y = 0; y < splashSpriteToptext->nHeight; y++)
			{
				short s = splashSpriteToptext->GetColour(x, y);

				if (s > 0) s--;

				splashSpriteToptext->SetColour(x, y, s);
				splashSpriteToptext->SetGlyph(x, y, PIXEL_HALF);

			}
			
			for (int x = 0; x < splashSpriteDesk->nWidth; x++) for (int y = 0; y < splashSpriteDesk->nHeight; y++)
			{
				short s = splashSpriteDesk->GetColour(x, y);

				if (s > 0) s--;

				splashSpriteDesk->SetColour(x, y, s);
				splashSpriteDesk->SetGlyph(x, y, PIXEL_HALF);

			}
			
			for (int x = 0; x < splashSpriteMonitor->nWidth; x++) for (int y = 0; y < splashSpriteMonitor->nHeight; y++)
			{
				short s = splashSpriteMonitor->GetColour(x, y);

				if (s > 0) s--;

				splashSpriteMonitor->SetColour(x, y, s);
				splashSpriteMonitor->SetGlyph(x, y, PIXEL_HALF);

			}

			for (int x = 0; x < spriteBtnPlay->nWidth; x++) for (int y = 0; y < spriteBtnPlay->nHeight; y++)
			{
				short s = spriteBtnPlay->GetColour(x, y);

				if (s > 0) s--;

				spriteBtnPlay->SetColour(x, y, s);
				spriteBtnPlay->SetGlyph(x, y, PIXEL_HALF);

			}

			for (int x = 0; x < spriteBtnSettings->nWidth; x++) for (int y = 0; y < spriteBtnSettings->nHeight; y++)
			{
				short s = spriteBtnSettings->GetColour(x, y);

				if (s > 0) s--;

				spriteBtnSettings->SetColour(x, y, s);
				spriteBtnSettings->SetGlyph(x, y, PIXEL_HALF);

			}
		}

		if (bEnding)
		{
			if ((int)lEndingLeft == -500)
			{
				tp1 = std::chrono::system_clock::now();

				float fWaitTime = 0.0f;

				while (fWaitTime < 1.0f)
				{
					// Handle Timing
					tp2 = std::chrono::system_clock::now();
					std::chrono::duration<float> elapsedTime = tp2 - tp1;
					tp1 = tp2;

					fWaitTime += elapsedTime.count();

					odprintf("%f", fWaitTime);

					Sleep(rand()* rand() % ((rand() % 10 + 1) * 10));
					SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Stop();

					Sleep(rand()* rand() % ((rand() % 10 + 1) * 10));
					SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Play(0, 0, 0);

					Sleep(rand()* rand() % ((rand() % 10 + 1) * 10));
					SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Stop();

					Sleep(rand()* rand() % ((rand() % 10 + 1) * 10));
					SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Play(0, 0, 0);

				}


				*currentSceneIndex = 1;
			}
			else

			lEndingLeft--;

			odprintf("%i", (int) lEndingLeft);

		}

		return true;
	}

	bool End() override
	{
		SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Stop();

		SoundMacro->vSounds[iIntroSoundIndex]->lpSecondaryBuffer->Release();

		return true;
	}
};

#endif