#ifndef PCMATRIX_H
#define PCMATRIX_H

#include <iostream>
#include <string.h>
#include <vector>

#include "program.h"
#include "sprite.h"

class PcMatrix : public PcProgram
{
public:
	struct sStreamer
	{
		int nColumn = 0;
		float fPosition = 0;
		float fSpeed = 0;
		wstring sText;
	};

	list<sStreamer> listStreamers;

	int nMaxStreamers = 300;

	wchar_t RandomCharacter()
	{
		return (wchar_t)(rand() % 0x1EF + 0x00C0);
		return (wchar_t)(rand() % 93 + 33); // Random ASCII
	}

	void PrepareStreamer(sStreamer* s)
	{
		s->nColumn = rand() % 80;
		s->fPosition = 0;
		s->fSpeed = rand() % 40 + 5;
		s->sText.clear();

		int nStreamerLength = rand() % 80 + 10;
		for (int i = 0; i < nStreamerLength; i++)
			s->sText.append(1, RandomCharacter());

		//s->sText = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	}

public:
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;

	int Initalize() override
	{
		windowSprite = new olcSprite(80, 26);										//Setup the default window to display

		for (int i = 0; i < windowSprite->nWidth; i++)								//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_BLACK);

			}

		onTaskbar = true;
	
		spriteIcon = new olcSprite(L"resources\\pcmatrixicon.png.spr");

		CenterWindow();

		return 0;
	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		// Clear Screen
		SpriteFill(windowSprite, 0, 0, windowSprite->nWidth, windowSprite->nHeight, PIXEL_SOLID, 0);

		for (auto& s : listStreamers)
		{
			s.fPosition += fElapsedTotal * s.fSpeed;
			for (int i = 0; i < s.sText.size(); i++)
			{
				// If you hate ternary operators do this
				// start ->
				short col = s.fSpeed < 15.0f ? FG_DARK_GREEN : FG_GREEN; // ;-)
				if (i == 0)
					col = FG_WHITE;
				else
					if (i <= 3)
						col = FG_GREY;

				int nCharIndex = (i - (int)s.fPosition) % s.sText.size();

				SpriteDraw(windowSprite, s.nColumn, (int)s.fPosition - i, s.sText[nCharIndex], col);
				// <- end

				// If you like them, do this!
				//Draw(s.nColumn, (int)s.fPosition - i, s.sText[(abs(i - (int)s.fPosition) % s.sText.size())], i == 0 ? FG_WHITE : i<=3 ? FG_GREY: i>=s.sText.size()-2 || s.fSpeed < 15.0f? FG_DARK_GREEN: FG_GREEN);

				// Missed this out on live stream, occasionally glitch a character
				if (rand() % 1000 < 5)
					s.sText[i] = RandomCharacter();
			}

			if (s.fPosition - s.sText.size() >= windowSprite->nHeight)
				PrepareStreamer(&s);

		}

		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;

			for (int i = 0; i < windowSprite->nWidth; i++)								//Clear the window sprite
				for (int j = 0; j < windowSprite->nHeight; j++)
				{
					windowSprite->SetGlyph(i, j, PIXEL_SOLID);

					if (j > 1)
						windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
					else
						windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

				}

			windowSprite->SetGlyph(78, 0, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(78, 0, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(78, 1, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(78, 1, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(77, 0, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(77, 0, BG_CYAN | FG_RED);

			windowSprite->SetGlyph(77, 1, PIXEL_THREEQUARTERS);
			windowSprite->SetColour(77, 1, BG_CYAN | FG_RED);
		}

	
		//Test if the mouse is inside the window
		if (mouseX >= windowPosition.u && mouseX <= windowPosition.u + windowSprite->nWidth)
		{
			if (mouseY >= windowPosition.v && mouseY <= windowPosition.v + windowSprite->nHeight)
			{
				//Check if the mouse 
				if (olc->GetMouse(0).bHeld)
				{
					//Test to see if we are dragging the mouse
					if (true)
						if (mouseX != lastMouseX || mouseY != lastMouseY)										//We are dragging
						{
							if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)				//We are moving the window
							{
								isdraggingwindow = true;
								isNeeded = true;
							}
						}
						else																					//We are clicking
						{
							if (mouseX == windowPosition.u + 77 || mouseX == windowPosition.u + 78)
							{
								if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
								{
									exitProgram = true;

									for (int i = 0; i < 80; i++)												//Clear the window sprite
									{
										for (int j = 0; j < 25; j++)
										{
											windowSprite->SetGlyph(i, j, PIXEL_HALF);
											windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
										}
									}
								}
							}
						}
				}
				if (olc->GetMouse(0).bReleased)
				{
					if (isdraggingwindow)
					{
						isdraggingwindow = false;
						isNeeded = false;
					}

					if (exitProgram == true)
					{
						onDisplay = false;
						exitProgram = false;
					}
				}

			}
		}

		if (isdraggingwindow)
		{
			windowPosition.u += mouseX - lastMouseX;
			windowPosition.v += mouseY - lastMouseY;
		}

		return 0;
	}

	int Display() override
	{
		onDisplay = true;

		for (int n = 0; n < nMaxStreamers; n++)
		{
			sStreamer s;
			PrepareStreamer(&s);
			s.fSpeed = 0.01;
			listStreamers.push_back(s);
		}

		return 0;
	}

	int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) override
	{
		if (onDisplay == false)
		{
			onDisplay = true;

			fBootedTime = fElapsedTotal;

			started = false;

			CenterWindow();
		}

		for (int n = 0; n < nMaxStreamers; n++)
		{
			sStreamer s;
			PrepareStreamer(&s);
			s.fSpeed = 0.01;
			listStreamers.push_back(s);
		}

		return 0;
	}

	int MouseUpdate(int x, int y) override
	{
		this->lastMouseX = mouseX;
		this->lastMouseY = mouseY;
		this->mouseX = x;
		this->mouseY = y;

		return 0;
	}

};

#endif