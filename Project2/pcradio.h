//#ifndef PCRADIO_H
//#define PCRADIO_H
//
//#include <iostream>
//#include <string.h>
//#include <vector>
//
//#include "fonthandler.h"
//#include "program.h"
//#include "olcConsoleGameEngine.h"
//
//class PcRadio : public PcProgram
//{
//	int lastMouseX = 0, lastMouseY = 0;
//	int mouseX = 0, mouseY = 0;
//	bool exitProgram = false;
//	bool isdraggingwindow = false;
//	bool started = false;
//	float fBootedTime = 0.0f;
//	bool bIsFront = false;
//
//	std::vector<PcButton> buttons;
//
//	int Initalize() override
//	{
//		windowSprite = new olcSprite(250, 75);										//Setup the default window to display
//
//		for (int i = 0; i < windowSprite->nWidth; i++)								//Clear the window sprite
//			for (int j = 0; j < windowSprite->nHeight; j++)
//			{
//				windowSprite->SetGlyph(i, j, PIXEL_HALF);
//
//				windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
//
//			}
//
//		onTaskbar = true;
//
//		CenterWindow();
//
//		buttons.push_back({ 25,25,25,5, FontHandler::GetSprite("Exit", BG_GREY | FG_BLACK),std::bind(&PcRadio::Exit, this) });			//Create a basic button
//
//		spriteIcon = new olcSprite(L"resources\\pcradioicon.png.spr");
//
//		return 0;
//	}
//
//	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
//	{
//		int relMousePosX = mouseX - windowPosition.u;
//		int relMousePosY = mouseY - windowPosition.v - 2;
//
//		bIsFront = front;
//
//		//Cool loading effect
//		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
//		{
//			started = true;
//		}
//		else
//			Draw(olc);
//
//		if (IfMouseClick(mouseX, mouseY, olc))
//		{
//			//Cycle through all the buttons in order to see if we click one
//			for (int i = 0; i < buttons.size(); i++)
//			{
//				if (relMousePosX >= buttons[i].x && relMousePosX <= buttons[i].x + buttons[i].iwidth)						//Test to see if the cursor is inside the button & we havent hovered any other buttons so far
//				{
//					if (relMousePosY >= buttons[i].y && relMousePosY <= buttons[i].y + buttons[i].iheight)					//...
//					{
//						buttons[i].function();
//						break;
//					}
//				}
//			}
//		}
//
//		//Check if the mouse is held
//		else if (olc->GetMouse(0).bHeld)
//		{
//			//Test to see if we are dragging the mouse
//			if (front)
//				if (mouseX != lastMouseX || mouseY != lastMouseY)															//We are dragging
//				{
//					if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)								//We are moving the window
//					{
//						isdraggingwindow = true;
//						isNeeded = true;
//					}
//				}
//				else																									//We are clicking
//				{
//					if (mouseX == windowPosition.u + (windowSprite->nWidth - 3) || mouseX == windowPosition.u + (windowSprite->nWidth - 2))
//					{
//						if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
//						{
//							exitProgram = true;
//
//							for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
//							{
//								for (int j = 0; j < windowSprite->nHeight; j++)
//								{
//									windowSprite->SetGlyph(i, j, PIXEL_HALF);
//									windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
//								}
//							}
//						}
//					}
//				}
//		}
//
//		if (olc->GetMouse(0).bReleased)
//		{
//			if (isdraggingwindow)
//			{
//				isdraggingwindow = false;
//				isNeeded = false;
//			}
//
//			if (exitProgram == true)
//			{
//				onDisplay = false;
//				exitProgram = false;
//			}
//		}
//
//		if (isdraggingwindow)
//		{
//			windowPosition.u += mouseX - lastMouseX;
//			windowPosition.v += mouseY - lastMouseY;
//		}
//
//		return 0;
//	}
//
//	int Exit()
//	{
//		exitProgram = true;
//
//		return 0;
//	}
//
//	int Display() override
//	{
//
//		for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
//			for (int j = 0; j < windowSprite->nHeight; j++)
//			{
//				windowSprite->SetGlyph(i, j, PIXEL_SOLID);
//
//				if (j > 1)
//					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
//				else
//					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);
//
//			}
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);
//
//		for (int b = 0; b < buttons.size(); b++)															//Draw and cycle through all the buttons
//		{
//			for (int i = 0; i < buttons[b].iwidth; i++)														//Draw the sprite
//				for (int j = 0; j < buttons[b].iheight; j++)
//				{
//					windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 2, PIXEL_HALF);				//Set the color of the sprite
//					windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 2, BG_GREY | FG_WHITE);
//				}
//		}
//
//		return 0;
//	}
//
//	int Draw(olcConsoleGameEngine* e)
//	{
//		int relMousePosX = mouseX - windowPosition.u;
//		int relMousePosY = mouseY - windowPosition.v - 2;
//
//		for (int i = 0; i < windowSprite->nWidth; i++)															//Clear the window sprite
//			for (int j = 0; j < windowSprite->nHeight; j++)
//			{
//				windowSprite->SetGlyph(i, j, PIXEL_SOLID);
//
//				if (j > 1)
//					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
//				else
//					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);
//
//			}
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);
//
//		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
//		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);
//
//		//Cycle through all the buttons in order to see if we click one
//		bool bInside = false;
//
//		for (int b = 0; b < buttons.size(); b++)
//		{
//			if (onDisplay)
//				if (relMousePosX >= buttons[b].x && relMousePosX <= buttons[b].x + buttons[b].iwidth && !bInside && bIsFront)	//Test to see if the cursor is inside the button & we havent hovered any other buttons so far
//				{
//					if (relMousePosY >= buttons[b].y && relMousePosY <= buttons[b].y + buttons[b].iheight)						//...
//					{
//						bInside = true;
//
//						for (int i = 0; i < buttons[b].iwidth; i++)																//Draw the sprite
//							for (int j = 0; j < buttons[b].iheight; j++)
//							{
//								windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 3, PIXEL_HALF);						//Set the color of the sprite
//								windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 3, BG_GREY | FG_WHITE);
//							}
//
//						int sx = buttons[b].x + (buttons[b].iwidth / 2) - (buttons[b].spriteInside->nWidth / 2.5);
//						int sy = buttons[b].y + (buttons[b].iheight / 2) - (buttons[b].spriteInside->nHeight / 2.5) + 4;
//
//						AddSpriteToWindow(buttons[b].spriteInside, sx, sy);
//
//						break;
//					}
//
//				}
//
//			if (!bInside)
//			{
//				for (int i = 0; i < buttons[b].iwidth; i++)																	//Draw the sprite
//					for (int j = 0; j < buttons[b].iheight; j++)
//					{
//						windowSprite->SetGlyph(buttons[b].x + i, buttons[b].y + j + 2, PIXEL_HALF);							//Set the color of the sprite
//						windowSprite->SetColour(buttons[b].x + i, buttons[b].y + j + 2, BG_GREY | FG_WHITE);
//					}
//
//				int sx = buttons[b].x + (buttons[b].iwidth / 2) - (buttons[b].spriteInside->nWidth / 2.5);
//				int sy = buttons[b].y + (buttons[b].iheight / 2) - (buttons[b].spriteInside->nHeight / 2.5) + 3;
//
//				AddSpriteToWindow(buttons[b].spriteInside, sx, sy);
//			}
//		}
//
//		return 0;
//	}
//
//	int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) override
//	{
//		if (onDisplay == false)
//		{
//			onDisplay = true;
//
//			fBootedTime = fElapsedTotal;
//
//			started = false;
//
//			CenterWindow();
//		}
//
//		return 0;
//	}
//
//	int MouseUpdate(int x, int y) override
//	{
//		this->lastMouseX = mouseX;
//		this->lastMouseY = mouseY;
//		this->mouseX = x;
//		this->mouseY = y;
//
//		return 0;
//	}
//
//};
//
//#endif

#ifndef PCMONKEY_H
#define PCMONKEY_H

#include <iostream>
#include <string.h>
#include <vector>

#include "program.h"
#include "objecth.h"
#include "debug.h"
#include "sprite.h"
#include "render.h"
#include "olcConsoleGameEngine.h"

class PcMonley : public PcProgram
{
public:
	struct MonkeyLevel
	{
		olcSprite*			sprBackground=nullptr, * sprIntro=nullptr;
		vector<rectangle>	platforms;
		vector<rectangle>	spikes;
		rectangle			rPlayer, rLastPlayer, rWin;
		vec2d				vSpace, vSpawn;
		bool				bMaingame = false;
		bool				bWinScreen = false;
	};

	olcSprite*				sprBanana, * sprSpace, * sprCongrats, * sprIntro, * sprControls, * sprLastTransition;
	PcSound*				wavIntro, * wavMain, * wavJump, * wavCongrat, * wavDeath;
	bool					bCanJump = false, bIntro = true, bControls = false, bEnded = false;
	vec2d					vPlayerVelocity, vPlayerAcceleration;
	vector<MonkeyLevel>		vMonkeyLevels;
	vec3d					vPerfectCamera = vec3d(-5.920451, 0.500000, 6.504760); 
	float					fPitch	= 0.209572; 
	float					fYaw	= -2.085016;
	size_t					sCurrentLevel = 0;
	float					fJumpPlay = 0, fCongratsSound = 0, fwavDeath = 0, fElapsedTotal = 0, fElapsedLast = 0;

public:
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;
	bool bIsFront = false;

	int Initalize() override
	{
		windowSprite = new olcSprite(250, 75 + 2);										//Setup the default window to display

		for (int i = 0; i < windowSprite->nWidth; i++)									//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);

			}

		onTaskbar = true;

		CenterWindow();

		spriteIcon = new olcSprite(L"resources\\iconmonkey.png.spr");

		return 0;
	}


	void HandleSounds(float fElapsedTime)
	{
		if (fJumpPlay < .5)
		{
			fJumpPlay += fElapsedTime;

			if (fJumpPlay > .5)
			{
				wavJump->wavData->lpSecondaryBuffer->Stop();
				wavJump->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
			}
		}

		if (fCongratsSound < 1)
		{
			fCongratsSound += fElapsedTime;

			if (fCongratsSound > 1)
			{
				wavCongrat->wavData->lpSecondaryBuffer->Stop();
				wavCongrat->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
			}
		}

		if (fwavDeath < .5)
		{
			fwavDeath += fElapsedTime;

			if (fwavDeath > .5)
			{
				wavDeath->wavData->lpSecondaryBuffer->Stop();
				wavDeath->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
			}
		}

	}

	void HandleDeath()
	{
		wavDeath->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
		wavDeath->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		fwavDeath = 0;
		vMonkeyLevels[sCurrentLevel].rPlayer.offset = vMonkeyLevels[sCurrentLevel].vSpawn;
	}

	void DrawMain()
	{
		if (!bIntro)
		{
			if (vMonkeyLevels[sCurrentLevel].bMaingame == true)
			{
				if (!vMonkeyLevels[sCurrentLevel].bWinScreen)
				{
					SpriteFill(windowSprite, 0, 0 + 2, windowSprite->nWidth, windowSprite->nHeight, PIXEL_SOLID, FG_WHITE | BG_WHITE);
					SpriteDrawSprite(windowSprite, 0, 0 + 2, vMonkeyLevels[sCurrentLevel].sprBackground);

					for (auto r : vMonkeyLevels[sCurrentLevel].platforms)
					{
						/*
						SpriteDrawTriangle(windowSprite, r.p[0].u, r.p[0].v + 2, r.p[1].u, r.p[1].v + 2, r.p[2].u, r.p[2].v + 2, PIXEL_HALF, BG_BLACK | FG_GREEN);
						SpriteDrawTriangle(windowSprite, r.p[3].u, r.p[3].v + 2, r.p[1].u, r.p[1].v + 2, r.p[2].u, r.p[2].v + 2, PIXEL_HALF, BG_BLACK | FG_GREEN);*/
						vec2d vm = r.p[0];
						vm.x = floorf(vm.x);
						vm.y = floorf(vm.y);

						for (size_t s1 = 0; s1 < r.p[1].x - r.p[0].x; s1++)
						{
							for (size_t s2 = 0; s2 < r.p[2].y - r.p[0].y; s2++)
							{
								vec2d sv = vm; sv.x += s1; sv.y += s2;

								SpriteDraw(windowSprite, sv.x, sv.y + 3, PIXEL_HALF, BG_BLACK | FG_GREEN);
							}
						}

					}

					for (auto r : vMonkeyLevels[sCurrentLevel].spikes)
					{
						vec2d vt0 = r.p[0] - r.p[1];
						vt0 /= 6.0;
						vt0.v = 0;
						vec2d vt1 = (vt0 * 1) + r.p[0];
						vec2d vt2 = (vt0 * 2) + r.p[0];
						vec2d vt3 = (vt0 * 3) + r.p[0];
						vec2d vt4 = (vt0 * 4) + r.p[0];
						vec2d vt5 = (vt0 * 5) + r.p[0];
						vec2d vt6 = (vt0 * 6) + r.p[0];
						vt0 = r.p[0];

						vec2d vb0 = r.p[2] - r.p[3];
						vb0 /= 6.0;
						vb0.v = 0;
						vec2d vb1 = (vb0 * 1) + r.p[2];
						vec2d vb2 = (vb0 * 2) + r.p[2];
						vec2d vb3 = (vb0 * 3) + r.p[2];
						vec2d vb4 = (vb0 * 4) + r.p[2];
						vec2d vb5 = (vb0 * 5) + r.p[2];
						vec2d vb6 = (vb0 * 6) + r.p[2];
						vb0.y+=2; vb1.y++; vb2.y++; vb3.y++; vb4.y++; vb5.y++; vb6.y++;
						vb0 = r.p[2];

						SpriteDrawTriangle(windowSprite, vb0.u, vb0.v + 2, vt1.u, vt1.v + 2, vb2.u, vb2.v + 2, PIXEL_THREEQUARTERS, FG_BLACK | BG_WHITE);
						SpriteDrawTriangle(windowSprite, vb2.u, vb2.v + 2, vt3.u, vt3.v + 2, vb4.u, vb4.v + 2, PIXEL_THREEQUARTERS, FG_BLACK | BG_WHITE);
						SpriteDrawTriangle(windowSprite, vb4.u, vb4.v + 2, vt5.u, vt5.v + 2, vb6.u, vb6.v + 2, PIXEL_THREEQUARTERS, FG_BLACK | BG_WHITE);

						//DrawTriangle(r.p[0].u, r.p[0].v, r.p[1].u, r.p[1].v, r.p[2].u, r.p[2].v, PIXEL_HALF, BG_BLACK | FG_GREEN);
						//DrawTriangle(r.p[3].u, r.p[3].v, r.p[1].u, r.p[1].v, r.p[2].u, r.p[2].v, PIXEL_HALF, BG_BLACK | FG_GREEN);
					}

					SpriteDrawTriangle(windowSprite, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[0].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[0].v + 3, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[1].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[1].v + 3, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[2].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[2].v + 3, PIXEL_HALF, BG_BLACK | FG_RED);
					SpriteDrawTriangle(windowSprite, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[3].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[3].v + 3, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[1].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[1].v + 3, vMonkeyLevels[sCurrentLevel].rPlayer.offset.u + vMonkeyLevels[sCurrentLevel].rPlayer.p[2].u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v + vMonkeyLevels[sCurrentLevel].rPlayer.p[2].v + 3, PIXEL_HALF, BG_BLACK | FG_RED);

					SpriteDrawSprite(windowSprite, 220.301498, 11.877110 + 2, sprBanana);

					vMonkeyLevels[sCurrentLevel].rLastPlayer = vMonkeyLevels[sCurrentLevel].rPlayer;
				}
				else
				{
					SpriteFill(windowSprite, 0, 0 + 2, windowSprite->nWidth, windowSprite->nHeight, PIXEL_SOLID, FG_WHITE | BG_WHITE);

					for (int i = 0; i < vMonkeyLevels[sCurrentLevel].sprBackground->nWidth; i++)
						for (int j = 0; j < vMonkeyLevels[sCurrentLevel].sprBackground->nWidth; j++)
						{
							SpriteDraw(windowSprite, i, j + 2, L' ', vMonkeyLevels[sCurrentLevel].sprBackground->GetColour(i, j));
						}

					float tempf = fElapsedTotal;
					while (tempf > 1.0f) tempf -= 1.0f;

					if (tempf < 0.50f)
						SpriteDrawSprite(windowSprite, (windowSprite->nWidth / 2) - (sprSpace->nWidth / 2), windowSprite->nHeight - (sprSpace->nHeight * 2) + 2, sprSpace);

					SpriteDrawSprite(windowSprite, (windowSprite->nWidth / 2) - (sprCongrats->nWidth / 2), 10 + 2, sprCongrats);

				}
			}
			else
			{
				SpriteDrawSprite(windowSprite, 0, 0 + 2, vMonkeyLevels[sCurrentLevel].sprIntro);

				float tempf = fElapsedTotal;
				while (tempf > 1.0f) tempf -= 1.0f;

				if (tempf < 0.50f)
					SpriteDrawSprite(windowSprite, vMonkeyLevels[sCurrentLevel].vSpace.u, vMonkeyLevels[sCurrentLevel].vSpace.v + 2, sprSpace);
			}
		}
		else
		{
			if (!bControls) SpriteDrawSprite(windowSprite, 0, 2, sprIntro);
			else SpriteDrawSprite(windowSprite, 0, 2, sprControls);
		}

		if (fElapsedLast > 1) 	SpriteDrawSprite(windowSprite, 0, 3, sprLastTransition);
	}

	void UpdateMain(olcConsoleGameEngine* olc, float fElapsedTime)
	{
		if (exitProgram == true)return;
		HandleSounds(fElapsedTime);

		if (!bIntro)
		{
			if (olc->GetKey(L'Q').bPressed)
			{
				odprintf("%f %f", vMonkeyLevels[sCurrentLevel].rPlayer.offset.u, vMonkeyLevels[sCurrentLevel].rPlayer.offset.v);
				odprintf("%d %d", olc->GetMouseX(), olc->GetMouseY());
			}

			if (vMonkeyLevels[sCurrentLevel].bMaingame == true)
			{
				if (sCurrentLevel == vMonkeyLevels.size() - 1)
				{
					fElapsedLast += fElapsedTime;
					odprintf("%f", fElapsedLast);
					if (fElapsedLast > 10.0f)
					{
						Exit();
						bEnded = true;
						return;
					}
				}

				if (vMonkeyLevels[sCurrentLevel].bWinScreen)
				{
					if (olc->GetKey(VK_SPACE).bPressed)
					{
						sCurrentLevel++;
						bCanJump = false;
						vPlayerVelocity = vec2d(0, 0);
					}
				}
				else
				{
					if (olc->GetKey(L'D').bHeld)			vPlayerVelocity.u = -9 * 3 * 2;
					else if (olc->GetKey(L'A').bHeld)	vPlayerVelocity.u = 9 * 3 * 2;
					else							vPlayerVelocity.u = 0;

					if (olc->GetKey(L'W').bPressed && bCanJump)
					{
						vPlayerVelocity.v = 47.5;
						bCanJump = false;
						fJumpPlay = 0;
						wavJump->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
						wavJump->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
					}

					vPlayerVelocity += vPlayerAcceleration * fElapsedTime;

					vMonkeyLevels[sCurrentLevel].rPlayer.offset.u -= vPlayerVelocity.u * fElapsedTime;

					rectangle rScreen = rectangle(0, 3, windowSprite->nWidth, windowSprite->nHeight - 3);

					if (!vMonkeyLevels[sCurrentLevel].rPlayer.bFastCollidsWith(rScreen))
					{
						vMonkeyLevels[sCurrentLevel].rPlayer.offset = vMonkeyLevels[sCurrentLevel].rLastPlayer.offset;
						if (vMonkeyLevels[sCurrentLevel].rPlayer.offset.y >= windowSprite->nHeight)
							HandleDeath();
					}

					for (auto r : vMonkeyLevels[sCurrentLevel].platforms)
					{
						if (r.bFastCollidsWith(vMonkeyLevels[sCurrentLevel].rPlayer))
						{
							vMonkeyLevels[sCurrentLevel].rPlayer.offset.u = vMonkeyLevels[sCurrentLevel].rLastPlayer.offset.u;
							vPlayerVelocity.u = 0;
						}
					}

					vMonkeyLevels[sCurrentLevel].rPlayer.offset.v -= vPlayerVelocity.v * fElapsedTime;

					for (auto r : vMonkeyLevels[sCurrentLevel].platforms)
					{
						if (r.bFastCollidsWith(vMonkeyLevels[sCurrentLevel].rPlayer))
						{
							vMonkeyLevels[sCurrentLevel].rPlayer.offset.v = vMonkeyLevels[sCurrentLevel].rLastPlayer.offset.v;
							vPlayerVelocity.v = 0;
							bCanJump = true;
						}
					}

					for (auto r : vMonkeyLevels[sCurrentLevel].spikes)
					{
						if (r.bFastCollidsWith(vMonkeyLevels[sCurrentLevel].rPlayer))
						{
							HandleDeath();
						}
					}

					if (vMonkeyLevels[sCurrentLevel].rPlayer.bFastCollidsWith(vMonkeyLevels[sCurrentLevel].rWin))
					{
						vMonkeyLevels[sCurrentLevel].bWinScreen = true;
						fCongratsSound = 0;
						wavCongrat->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
						wavCongrat->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
					}

				}
			}
			else
			{
				if (olc->GetKey(VK_SPACE).bPressed)vMonkeyLevels[sCurrentLevel].bMaingame = true;

				if (sCurrentLevel == vMonkeyLevels.size() - 1)
				{
					fElapsedLast = 0;
				}
			}
		}
		else
		{
			if ((olc->GetKey(VK_RETURN).bPressed || olc->GetKey(VK_SPACE).bPressed) && bControls == false)
			{
				bControls = true;
				wavIntro->wavData->lpSecondaryBuffer->Stop();
				wavMain->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			}
			else if ((olc->GetKey(VK_RETURN).bPressed || olc->GetKey(VK_SPACE).bPressed) && bControls == true)
			{
				bIntro = false;
			}
		}

	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		bIsFront = front;

		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;
		}
		else
			Draw(olc);

		//Check if the mouse is held
		if (olc->GetMouse(0).bHeld)
		{
			//Test to see if we are dragging the mouse
			if (front)
				if (mouseX != lastMouseX || mouseY != lastMouseY)															//We are dragging
				{
					if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)								//We are moving the window
					{
						isdraggingwindow = true;
						isNeeded = true;
					}
				}
				else																									//We are clicking
				{
					if (mouseX == windowPosition.u + (windowSprite->nWidth - 3) || mouseX == windowPosition.u + (windowSprite->nWidth - 2))
					{
						if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
						{
							Exit();

							for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
							{
								for (int j = 0; j < windowSprite->nHeight; j++)
								{
									windowSprite->SetGlyph(i, j, PIXEL_HALF);
									windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
								}
							}
						}
					}
				}
		}


		if (front)
		{
			UpdateMain(olc, fElapsed);
			if (fElapsedLast < 10.0f) DrawMain();
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

		if (isdraggingwindow)
		{
			windowPosition.u += mouseX - lastMouseX;
			windowPosition.v += mouseY - lastMouseY;
		}

		return 0;
	}

	int Exit()
	{
		//sprBanana		= new olcSprite(L"resources\\bananna.png.spr");
		//sprSpace		= new olcSprite(L"resources\\info space.png.spr");
		//sprCongrats		= new olcSprite(L"resources\\congrats.png.spr");
		//sprIntro		= new olcSprite(L"resources\\monkeyintro.png.spr");
		//sprControls		= new olcSprite(L"resources\\monkeycontrols.png.spr");

		//delete sprBanana;
		//delete sprSpace;
		//delete sprCongrats;
		//delete sprIntro;
		//delete sprControls;

		wavIntro->wavData->lpSecondaryBuffer->Stop();
		wavMain->wavData->lpSecondaryBuffer->Stop();
		wavJump->wavData->lpSecondaryBuffer->Stop();
		wavCongrat->wavData->lpSecondaryBuffer->Stop();
		wavDeath->wavData->lpSecondaryBuffer->Stop();

		vMonkeyLevels.clear();
		vMonkeyLevels.shrink_to_fit();

		vPlayerAcceleration.v = -60;

		exitProgram = true;

		return 0;
	}

	int Display() override
	{

		for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);


		return 0;
	}

	int Draw(olcConsoleGameEngine* e)
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		for (int i = 0; i < windowSprite->nWidth; i++)															//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);

		//Cycle through all the buttons in order to see if we click one
		bool bInside = false;

		return 0;
	}

	int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) override
	{
		if (bEnded) return -1;

		if (onDisplay == false)
		{
			if (fElapsedLast >= 10.0f)
			{
				pcnCurrent		= nullptr;
				pcnCorrupted	= nullptr;

				return 0;
			}

			onDisplay = true;

			fBootedTime = fElapsedTotal;

			started = false;

			CenterWindow();

			fJumpPlay = 0, fCongratsSound = 0, fwavDeath = 0, fElapsedTotal = 0, fElapsedLast = 0;

			sprBanana			= new olcSprite(L"resources\\bananna.png.spr");
			sprSpace			= new olcSprite(L"resources\\info space.png.spr");
			sprCongrats			= new olcSprite(L"resources\\congrats.png.spr");
			sprIntro			= new olcSprite(L"resources\\monkeyintro.png.spr");
			sprControls			= new olcSprite(L"resources\\monkeycontrols.png.spr");
			sprLastTransition	= new olcSprite(L"resources\\monkeytransition4.png.spr");

			wavIntro = new PcSound;
			wavMain = new PcSound;
			wavJump = new PcSound;
			wavCongrat = new PcSound;
			wavDeath = new PcSound;

			vPcSounds.push_back(wavIntro );
			vPcSounds.push_back(wavMain );
			vPcSounds.push_back(wavJump );
			vPcSounds.push_back(wavCongrat );
			vPcSounds.push_back(wavDeath );

			wavIntro->wavData	= SoundMacro->LoadSoundFileGetWave(L"resources\\monkeyintro.wav", false, true, false);
			wavMain->wavData	= SoundMacro->LoadSoundFileGetWave(L"resources\\monkeymain.wav", false, true, false);
			wavJump->wavData	= SoundMacro->LoadSoundFileGetWave(L"resources\\monkeyjump.wav", false, true, false);
			wavCongrat->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\monkeycongrats.wav", false, true, false);
			wavDeath->wavData	= SoundMacro->LoadSoundFileGetWave(L"resources\\monkeydeath.wav", false, true, false);

			if (wavIntro)
			{
				wavIntro->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			}

			vMonkeyLevels.push_back(MonkeyLevel());
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(0, 69, 250, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(25, 59 - 5, 5, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(57, 40 + 3 + 3, 10+5, 2));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(164 - 5 - 5 - 5 - 5, 31 + 8, 10 + 5 + 5 + 5 + 5, 2));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(218, 21 + 8, 10, 2));
			vMonkeyLevels[vMonkeyLevels.size() - 1].rPlayer = rectangle(0, 0, 1, 2);
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprBackground = new olcSprite(L"resources\\monkeybackground1.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprIntro = new olcSprite(L"resources\\monkeytransition1.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].rWin = rectangle(220.301498, 11.877110, sprBanana->nWidth, sprBanana->nHeight);
			vMonkeyLevels[vMonkeyLevels.size() - 1].vSpace = vec2d(36 + 10 + 5, 51);

			vMonkeyLevels.push_back(MonkeyLevel());
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(95, 55, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(0, 69, 250 / 3, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(134, 39, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(154, 44 - (44 - 17), 168 - 154, 44 - 17));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(95, 55 - 23, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(183, 0, 168 - 154, 44 - 17));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(177, 51, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(228, 36, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].rPlayer = rectangle(0, 0, 1, 2);
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprBackground = new olcSprite(L"resources\\monkeybackground1.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprIntro = new olcSprite(L"resources\\monkeytransition2.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].rWin = rectangle(220.301498, 11.877110, sprBanana->nWidth, sprBanana->nHeight);
			vMonkeyLevels[vMonkeyLevels.size() - 1].vSpace = vec2d(36 + 10 + 5 + 30, 51);

			vMonkeyLevels.push_back(MonkeyLevel());
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(174, 16 + 5, 245 - 124, 3));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(124, 35, 141 - 124, 41 - 35));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(157 + 20, 50, 157 - 189, 2));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(0, 36, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(67, 65, 75, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].spikes.push_back(rectangle(86, 61, 115 - 86, 64 - 61));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(40, 21, 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(73, 0, 2, 50));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(40, 26, 2, 56 - 26));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(40, 26 + (56 - 26), 20, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].spikes.push_back(rectangle(60, 33, 12, 3));
			vMonkeyLevels[vMonkeyLevels.size() - 1].platforms.push_back(rectangle(60, 33 + 3, 12, 3));
			vMonkeyLevels[vMonkeyLevels.size() - 1].rPlayer = rectangle(0, 0, 1, 2);
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprBackground = new olcSprite(L"resources\\monkeybackground1.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprIntro = new olcSprite(L"resources\\monkeytransition3.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].rWin = rectangle(220.301498, 11.877110, sprBanana->nWidth, sprBanana->nHeight);
			vMonkeyLevels[vMonkeyLevels.size() - 1].vSpace = vec2d(100, 60);

			vMonkeyLevels.push_back(MonkeyLevel());
			vMonkeyLevels[vMonkeyLevels.size() - 1].spikes.push_back(rectangle(0, 69, 250, 5));
			vMonkeyLevels[vMonkeyLevels.size() - 1].rPlayer = rectangle(0, 0, 1, 2);
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprBackground = new olcSprite(L"resources\\monkeybackground1.png.spr");
			vMonkeyLevels[vMonkeyLevels.size() - 1].sprIntro = RenderersPointerMacro(0)->GetSprite(250, 75, vec3d(-6.057152, 0.25, 6.853373), 0.089572, 4.092539);
			SpriteDrawSprite(vMonkeyLevels[vMonkeyLevels.size() - 1].sprIntro, 0, 0, sprLastTransition);
			vMonkeyLevels[vMonkeyLevels.size() - 1].rWin = rectangle(220.301498, 11.877110, sprBanana->nWidth, sprBanana->nHeight);
			vMonkeyLevels[vMonkeyLevels.size() - 1].vSpace = vec2d(36 + 10 + 5, 51);

			vPlayerAcceleration.v = -60;


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

class PcDriver : public PcProgram
{
public:
	olcSprite*			sprCar1, * sprCar2, * sprControls, * sprBranch, * sprBranch2, * sprHouse, * sprIntro, * sprEnding;
	rectangle			rPlayerPos;
	PcSound*			wavMain, * wavAmbience;
	vector<rectangle>	rEnemyPos;
	vector<vec2d>		rBranchPos;
	rectangle			rHouse;

	float fElapsedTotal = 0, fLastEnemySpawn = 0, fEnemySpeed = 0, fRoadSpeed = 0, fHouseY = 0;
	int numberoftimesdied = 9;
	bool bCanEnd = true, bIsOutside = false, bIntro = true, bOutsideTrigger = false;

public:
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;
	bool bIsFront = true;
	bool bEnded = false, bBooted = false;

	int Initalize() override
	{
		windowSprite = new olcSprite(250, 75 + 2);										//Setup the default window to display

		for (int i = 0; i < windowSprite->nWidth; i++)									//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);

			}

		onTaskbar = true;

		CenterWindow();

		spriteIcon = new olcSprite(L"resources\\iconcar.png.spr");

		return 0;
	}

	void OnDeath()
	{
		rEnemyPos.resize(0);
		rEnemyPos.shrink_to_fit();
		rPlayerPos.offset = vec2d(118.895958, 2.22059011);
		fEnemySpeed = 0;
		numberoftimesdied++;

		if (numberoftimesdied < 5)
			wavMain->wavData->SetVolume(5 - (numberoftimesdied), 5);
		else
			wavMain->wavData->SetVolume(0, 5);

		if (numberoftimesdied >= 9)
		{
			bIsOutside = true;
			bEnded = true;
		}

	}

	void UpdateMain(olcConsoleGameEngine* olc, float fElapsedTime)
	{
		if (bIntro)
		{
			SpriteDrawSprite(windowSprite, 0, 2, sprIntro);
			if (olc->GetKey(VK_RETURN).bPressed) bIntro = false;
		}
		else
		{
			fElapsedTotal += fElapsedTime;
			fRoadSpeed += fElapsedTime * (1 - (numberoftimesdied * .1));

			if (floorf(fLastEnemySpawn * 2) < floorf(fElapsedTotal * 2) && numberoftimesdied < 9)
			{
				rEnemyPos.push_back(rectangle(0, 0, sprCar2->nWidth * 2, sprCar2->nHeight * 2));
				rEnemyPos[rEnemyPos.size() - 1].offset = vec2d(25 + (rand() % (250 - 50)) - (sprCar2->nHeight * 2), 75);
				fLastEnemySpawn = fElapsedTotal;
			}

			if (numberoftimesdied < 6)
				SpriteFill(windowSprite, 0, 0 + 2, 250, 75 + 2, PIXEL_SOLID, FG_BLACK | BG_BLACK);
			else if (numberoftimesdied >= 6)
				SpriteFill(windowSprite, 0, 0 + 2, 250, 75 + 2, PIXEL_HALF, FG_GREY | BG_RED);

			if (numberoftimesdied < 4)
			{
				SpriteFill(windowSprite, 0, 0 + 2, 25, 75 + 2, PIXEL_HALF, FG_GREY | BG_DARK_GREY);
				SpriteFill(windowSprite, 250 - 25, 0 + 2, 250, 75 + 2, PIXEL_HALF, FG_GREY | BG_DARK_GREY);
			}
			else if (numberoftimesdied >= 4)
			{
				SpriteFill(windowSprite, 0, 0 + 2, 25, 75 + 2, PIXEL_HALF, FG_RED | BG_DARK_GREY);
				SpriteFill(windowSprite, 250 - 25, 0 + 2, 250, 75 + 2, PIXEL_HALF, FG_RED | BG_DARK_GREY);
			}

			int	isub = ((int)(fRoadSpeed * 250) % 20);
			float fsub = -20 + isub;

			if (numberoftimesdied < 9)
				for (int i = 0; i < 8; i++)
				{
					SpriteFill(windowSprite, (250 / 2) - 2, ((i * 20) + fsub) - 10 + 2, (250 / 2) + 2, (i * 20) + fsub + 2, PIXEL_HALF, FG_YELLOW | BG_BLACK);
				}

			if (olc->GetKey(L'W').bHeld) rPlayerPos.offset.y -= fElapsedTime * 5 * 2 * 2 * 2;
			if (olc->GetKey(L'S').bHeld) rPlayerPos.offset.y += fElapsedTime * 5 * 2 * 2 * 2;
			if (olc->GetKey(L'A').bHeld) rPlayerPos.offset.x -= fElapsedTime * 5 * 3 * 2 * 2 * 2;
			if (olc->GetKey(L'D').bHeld) rPlayerPos.offset.x += fElapsedTime * 5 * 3 * 2 * 2 * 2;

			if (numberoftimesdied >= 9)
				if (rHouse.bFastCollidsWith(rPlayerPos))
				{
					rPlayerPos.offset = vec2d(118.479828, 57.1731796);
				}

			if (fElapsedTotal < 0)
			{
				for (int i = 0; i < sprCar1->nWidth * 2; i++)
				{
					for (int j = 0; j < sprCar1->nHeight * 2; j++)
					{
						SpriteDraw(windowSprite, rPlayerPos.offset.x + i, rPlayerPos.offset.y + j + 2, sprCar1->GetGlyph(floor(i / 2), floor(j / 2)), sprCar1->GetColour(floor(i / 2), floor(j / 2)));
					}
				}

				SpriteDrawSprite(windowSprite, 0, 0 + 2, sprControls);

			}
			else
			{
				fEnemySpeed += fElapsedTime * .1;
				if (rPlayerPos.offset.x < 26) rPlayerPos.offset.x = 26;
				if (rPlayerPos.offset.x > 209) rPlayerPos.offset.x = 209;
				if (rPlayerPos.offset.y < 0) rPlayerPos.offset.y = 0;
				if (rPlayerPos.offset.y > 75 - rPlayerPos.p[3].y) rPlayerPos.offset.y = 75 - rPlayerPos.p[3].y;

				if (olc->GetKey(L'Q').bPressed) odprintf("%d %d", olc->GetMouseX(), olc->GetMouseY());

				for (int i = 0; i < sprCar1->nWidth * 2; i++)
				{
					for (int j = 0; j < sprCar1->nHeight * 2; j++)
					{
						SpriteDraw(windowSprite, rPlayerPos.offset.x + i, rPlayerPos.offset.y + j + 2, sprCar1->GetGlyph(floor(i / 2), floor(j / 2)), sprCar1->GetColour(floor(i / 2), floor(j / 2)));
					}
				}

				for (size_t t = 0; t < rEnemyPos.size(); t++)
				{
					rectangle* r = &rEnemyPos[t];
					r->offset.y -= fElapsedTime * 5 * 2 * 2 * 2 + fEnemySpeed;
					for (int i = 0; i < sprCar2->nWidth * 2; i++)
					{
						for (int j = 0; j < sprCar2->nHeight * 2; j++)
						{
							SpriteDraw(windowSprite, r->offset.x + i, r->offset.y + j + 2, sprCar2->GetGlyph(floor(i / 2), floor(j / 2)), sprCar2->GetColour(floor(i / 2), floor(j / 2)));
						}
					}
				}

				if (numberoftimesdied >= 9)
				{
					for (int i = 0; i < sprHouse->nWidth * 2.5; i++)
					{
						for (int j = 0; j < sprHouse->nHeight * 2.5; j++)
						{
							fHouseY += fElapsedTime * .0025;
							if (fHouseY > 0)
							{
								fHouseY = 0;
							}

							wavMain->wavData->SetVolume(-fHouseY * .1, (rHouse.p[3].y * 1.5));
							SpriteDraw(windowSprite, i, j + fHouseY + 2, sprHouse->GetGlyph(floor(i / 2.5), floor(j / 2.5)), sprHouse->GetColour(floor(i / 2.5), floor(j / 2.5)));
						}
					}

					SpriteDrawSprite(windowSprite, 0, 0, sprEnding);
				}

				if (numberoftimesdied >= 2)
					for (auto v : rBranchPos)
					{
						if (v.x <= 250 / 2)
							SpriteDrawSprite(windowSprite, v.x, v.y + 2, sprBranch);
						else if (v.x >= 250 / 2)
							SpriteDrawSprite(windowSprite, v.x - sprBranch2->nWidth, v.y + 2, sprBranch2);
					}



				for (auto r : rEnemyPos)
				{
					if (r.bFastCollidsWith(rPlayerPos)) OnDeath();
				}

			}

		}

		return;

	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		bIsFront = front;

		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;
		}
		else
			Draw(olc);

		//Check if the mouse is held
		if (olc->GetMouse(0).bHeld)
		{
			//Test to see if we are dragging the mouse
			if (front)
				if (mouseX != lastMouseX || mouseY != lastMouseY)															//We are dragging
				{
					if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)								//We are moving the window
					{
						isdraggingwindow = true;
						isNeeded = true;
					}
				}
				else																									//We are clicking
				{
					if (mouseX == windowPosition.u + (windowSprite->nWidth - 3) || mouseX == windowPosition.u + (windowSprite->nWidth - 2))
					{
						if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
						{
							if(bCanEnd)
							Exit();

							for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
							{
								for (int j = 0; j < windowSprite->nHeight; j++)
								{
									windowSprite->SetGlyph(i, j, PIXEL_HALF);
									windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
								}
							}
						}
					}
				}
		}


		if (front)
		{
			UpdateMain(olc, fElapsed);
			//if (fElapsedLast < 10.0f) DrawMain();
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

		if (isdraggingwindow)
		{
			windowPosition.u += mouseX - lastMouseX;
			windowPosition.v += mouseY - lastMouseY;
		}
		
		SpriteFill(windowSprite,0,0,250*.75,2, PIXEL_SOLID, BG_CYAN | FG_CYAN);

		return 0;
	}

	int Exit()
	{
		wavMain->wavData->lpSecondaryBuffer->Stop();
		wavAmbience->wavData->lpSecondaryBuffer->Stop();

		exitProgram = true;

		return 0;
	}

	int Display() override
	{

		for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);


		return 0;
	}

	int Draw(olcConsoleGameEngine* e)
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		for (int i = 0; i < windowSprite->nWidth; i++)															//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);

				if (j > 1)
					windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);
				else
					windowSprite->SetColour(i, j, BG_CYAN | FG_CYAN);

			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);

		//Cycle through all the buttons in order to see if we click one
		bool bInside = false;

		return 0;
	}

	int Boot(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal) override
	{
		if (bEnded) return -1;

		if (onDisplay == false)
		{
			onDisplay = true;

			fBootedTime = fElapsedTotal;

			started = false;

			CenterWindow();

			if (bBooted) return 0;

			this->fElapsedTotal = -10; fLastEnemySpawn = 0; fEnemySpeed = 0; fRoadSpeed = 0; fHouseY = 0;
			numberoftimesdied = 0;

			sprCar1 = new olcSprite(L"resources\\pcbattlecar1.png.spr");
			sprCar2 = new olcSprite(L"resources\\pcbattlecar2.png.spr");
			sprControls = new olcSprite(L"resources\\pcbattlecontrols.png.spr");
			sprBranch = new olcSprite(L"resources\\branch.png.spr");
			sprBranch2 = new olcSprite(L"resources\\branch2.png.spr");
			sprHouse = new olcSprite(L"resources\\pcbattlehouse.png.spr"); 
			sprIntro = new olcSprite(L"resources\\pcbattleintro.png.spr");
			sprEnding = new olcSprite(L"resources\\pcbattleending.png.spr");

			rPlayerPos = rectangle(0, 0, sprCar1->nWidth * 2, sprCar1->nHeight * 2);
			rHouse = rectangle(0, 0, sprHouse->nWidth * 2.5, sprHouse->nHeight * 2.5);
			rPlayerPos.offset = vec2d(118.895958, 2.22059011);
			fHouseY = -rHouse.p[3].y * 1.5;

			wavMain = new PcSound;
			wavAmbience = new PcSound;

			vPcSounds.push_back(wavMain);
			vPcSounds.push_back(wavAmbience);

			wavMain->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\pcbattlemain.wav", false, true, false);
			wavAmbience->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\pcbattleambience.wav", false, true, false);
			wavMain->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			wavAmbience->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
			wavAmbience->wavData->SetVolume(35.0f);

			rBranchPos.push_back({ 0.000000000, 2.00000000 });
			rBranchPos.push_back({ 3.00000000, 31.0000000 });
			rBranchPos.push_back({ 2.00000000, 36.0000000 });
			rBranchPos.push_back({ 0.000000000, 33.0000000 });
			rBranchPos.push_back({ 0.000000000, 62.0000000 });
			rBranchPos.push_back(vec2d(249 + 1, 0));
			rBranchPos.push_back(vec2d(249 + 1, 0));
			rBranchPos.push_back(vec2d(249 + 1, 21));
			rBranchPos.push_back(vec2d(249 + 1, 21));
			rBranchPos.push_back(vec2d(248 + 1, 46));
			rBranchPos.push_back(vec2d(248 + 1, 46));
			rBranchPos.push_back(vec2d(249 + 1, 47));
			rBranchPos.push_back(vec2d(249 + 1, 47));
			rBranchPos.push_back(vec2d(248 + 1, 1));
			rBranchPos.push_back(vec2d(248 + 1, 1));
			rBranchPos.push_back(vec2d(241 + 1, 27));
			rBranchPos.push_back(vec2d(241 + 1, 27));
			rBranchPos.push_back(vec2d(248 + 1, 32));
			rBranchPos.push_back(vec2d(248 + 1, 32));

			srand(time(NULL));

			bBooted = true;
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

class PcSnake : public PcProgram
{
public:
	enum SnakeMovement
	{
		SnakeMovement_none	= 0,
		SnakeMovement_left	= 1,
		SnakeMovement_right = 2,
		SnakeMovement_up	= 3,
		SnakeMovement_down	= 4
	};

public:
	vec2d			vFoodPos;
	vec2d			vSnakePos;
	vector<vec2d>	vSnakeTail;
	SnakeMovement	snakeCurrentMovement = SnakeMovement_none;
	olcSprite		* sprGameOver;
	PcSound			* wavMain, * wavEat, * wavDeath;
	size_t			scoreCurrent = 0, scoreHigh = 0;

	float	fLastTime = 0, fTotalTime = 0;
	float	fWavEat = 0, fWavDeath = 0;
	bool	bOnDeath = false;
	bool	bIsMoving = false;

public:
	int lastMouseX = 0, lastMouseY = 0;
	int mouseX = 0, mouseY = 0;
	bool exitProgram = false;
	bool isdraggingwindow = false;
	bool started = false;
	float fBootedTime = 0.0f;
	bool bIsFront = true;
	bool bEnded = false, bBooted = false;

	int Initalize() override
	{
		windowSprite = new olcSprite(250, 75 + 2);										//Setup the default window to display

		for (int i = 0; i < windowSprite->nWidth; i++)									//Clear the window sprite
			for (int j = 0; j < windowSprite->nHeight; j++)
			{
				windowSprite->SetGlyph(i, j, PIXEL_HALF);

				windowSprite->SetColour(i, j, BG_WHITE | FG_WHITE);

			}

		onTaskbar = true;

		CenterWindow();

		return 0;
	}

	void HandleSounds(float fElapsedTime)
	{
		if (fWavEat < 1)
		{
			fWavEat += fElapsedTime;

			if (fWavEat > 1)
			{
				wavEat->wavData->lpSecondaryBuffer->Stop();
				wavEat->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
			}
		}

		if (fWavDeath < .5)
		{
			fWavDeath += fElapsedTime;

			if (fWavDeath > .5)
			{
				wavDeath->wavData->lpSecondaryBuffer->Stop();
				wavDeath->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
			}
		}

	}

	void SetFood()
	{
		bool bSuitable = false;

		while (!bSuitable)
		{
			bSuitable = true;

			vFoodPos.x = rand() % ((int)(250 / 2.25)) + 1;
			vFoodPos.y = rand() % ((int)(75 / 2.25)) + 1;

			if (vSnakePos == vFoodPos)
				bSuitable = false;

			for (auto r : vSnakeTail)
				if (r == vFoodPos) bSuitable = false;
		}

		odprintf("%f %f", vFoodPos.x, vFoodPos.y);
	}

	void Reset()
	{
		vSnakeTail.resize(0);
		vSnakeTail.shrink_to_fit();
		vSnakePos = vec2d(20, 10);
		bOnDeath = false;
		snakeCurrentMovement = SnakeMovement_none;
		SetFood();
		SpriteFill(windowSprite, 0, 0+2, 250, 75 + 2, 0);
		wavMain->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		wavMain->wavData->lpSecondaryBuffer->SetCurrentPosition(0);
		
		if (scoreHigh > scoreCurrent) scoreHigh = scoreCurrent;

	}

	void OnDeath()
	{
		bOnDeath = true;
		wavMain->wavData->lpSecondaryBuffer->Stop();
		wavDeath->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		fWavDeath = 0;
	}

	bool UpdateMain(olcConsoleGameEngine* olc, float fElapsedTime)
	{
		HandleSounds(fElapsedTime);

		if (!bOnDeath)
		{
			fTotalTime += fElapsedTime;
			fLastTime += fElapsedTime;
			if (fLastTime < .05) return true;
			fLastTime = 0;

			if (olc->GetKey(L'A').bHeld)
			{
				snakeCurrentMovement = SnakeMovement_left;
			}
			else if (olc->GetKey(L'D').bHeld)
			{
				snakeCurrentMovement = SnakeMovement_right;
			}
			else if (olc->GetKey(L'W').bHeld)
			{
				snakeCurrentMovement = SnakeMovement_up;
			}
			else if (olc->GetKey(L'S').bHeld)
			{
				snakeCurrentMovement = SnakeMovement_down;
			}

			bIsMoving = true;
			switch (snakeCurrentMovement)
			{
			case SnakeMovement_left:
				vSnakePos.x--;
				break;
			case SnakeMovement_right:
				vSnakePos.x++;
				break;
			case SnakeMovement_up:
				vSnakePos.y--;
				break;
			case SnakeMovement_down:
				vSnakePos.y++;
				break;
			default:
				bIsMoving = false;
				break;
			}

			for (auto r : vSnakeTail)
			{
				SpriteDraw(windowSprite, r.u * 2 + 1, r.v * 2 + 1 + 2);
				SpriteDraw(windowSprite, r.u * 2, r.v * 2 + 1 + 2);
				SpriteDraw(windowSprite, r.u * 2 + 1, r.v * 2 + 2);
				SpriteDraw(windowSprite, r.u * 2, r.v * 2 + 2);

			}

			for (auto r : vSnakeTail)
			{
				if (r == vSnakePos && bIsMoving)
				{
					OnDeath();
				}
			}

			if (vSnakePos.x < 0 || vSnakePos.y < 0 || vSnakePos.x >(250 / 2) - 1 || vSnakePos.y >(75 / 2) - 1)
			{
				OnDeath();
			}

			if (vSnakePos == vFoodPos)
			{
				SetFood();
				wavEat->wavData->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
				fWavEat = 0;
			}

			vSnakeTail.push_back(vSnakePos);

			SpriteDraw(windowSprite, vSnakePos.u * 2 + 1, vSnakePos.v * 2 + 1 + 2, PIXEL_HALF, FG_BLUE | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2, vSnakePos.v * 2 + 1 + 2, PIXEL_HALF, FG_BLUE | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2 + 1, vSnakePos.v * 2 + 2, PIXEL_HALF, FG_BLUE | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2, vSnakePos.v * 2 + 2, PIXEL_HALF, FG_BLUE | BG_GREY);

			SpriteDraw(windowSprite, vFoodPos.u * 2 + 1, vFoodPos.v * 2 + 1 + 2, PIXEL_HALF, FG_GREEN | BG_GREY);
			SpriteDraw(windowSprite, vFoodPos.u * 2, vFoodPos.v * 2 + 1 + 2, PIXEL_HALF, FG_GREEN | BG_GREY);
			SpriteDraw(windowSprite, vFoodPos.u * 2 + 1, vFoodPos.v * 2 + 2, PIXEL_HALF, FG_GREEN | BG_GREY);
			SpriteDraw(windowSprite, vFoodPos.u * 2, vFoodPos.v * 2 + 2, PIXEL_HALF, FG_GREEN | BG_GREY);

		}
		else
		{
			for (auto r : vSnakeTail)
			{
				SpriteDraw(windowSprite, r.u * 2 + 1, r.v * 2 + 1 + 2);
				SpriteDraw(windowSprite, r.u * 2, r.v * 2 + 1 + 2);
				SpriteDraw(windowSprite, r.u * 2 + 1, r.v * 2 + 2);
				SpriteDraw(windowSprite, r.u * 2, r.v * 2 + 2);

			}

			SpriteDraw(windowSprite, vSnakePos.u * 2 + 1, vSnakePos.v * 2 + 1 + 2, PIXEL_HALF, FG_RED | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2, vSnakePos.v * 2 + 1 + 2, PIXEL_HALF, FG_RED | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2 + 1, vSnakePos.v * 2 + 2, PIXEL_HALF, FG_RED | BG_GREY);
			SpriteDraw(windowSprite, vSnakePos.u * 2, vSnakePos.v * 2 + 2, PIXEL_HALF, FG_RED | BG_GREY);

			SpriteDrawSprite(windowSprite, (250 / 2) - (sprGameOver->nWidth / 2), 10 + 2, sprGameOver);

			if (olc->GetKey(VK_RETURN).bPressed) Reset();

		}

		Display();

		return true;
	}

	int Update(olcConsoleGameEngine* olc, float fElapsed, float fElapsedTotal, bool front) override
	{
		int relMousePosX = mouseX - windowPosition.u;
		int relMousePosY = mouseY - windowPosition.v - 2;

		bIsFront = front;

		//Cool loading effect
		if (!started && onDisplay && fElapsedTotal - fBootedTime >= 0.05f)
		{
			started = true;
		}

		//Check if the mouse is held
		if (olc->GetMouse(0).bHeld)
		{
			//Test to see if we are dragging the mouse
			if (front)
				if (mouseX != lastMouseX || mouseY != lastMouseY)															//We are dragging
				{
					if (lastMouseY == windowPosition.v || lastMouseY == windowPosition.v + 1)								//We are moving the window
					{
						isdraggingwindow = true;
						isNeeded = true;
					}
				}
				else																									//We are clicking
				{
					if (mouseX == windowPosition.u + (windowSprite->nWidth - 3) || mouseX == windowPosition.u + (windowSprite->nWidth - 2))
					{
						if (mouseY == windowPosition.v || mouseY == windowPosition.v + 1)
						{
							Exit();

							for (int i = 0; i < windowSprite->nWidth; i++)												//Clear the window sprite
							{
								for (int j = 0; j < windowSprite->nHeight; j++)
								{
									windowSprite->SetGlyph(i, j, PIXEL_HALF);
									windowSprite->SetColour(i, j, BG_GREY | FG_DARK_GREY);
								}
							}
						}
					}
				}
		}


		if (front)
		{
			UpdateMain(olc, fElapsed);
			//if (fElapsedLast < 10.0f) DrawMain();
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

		if (isdraggingwindow)
		{
			windowPosition.u += mouseX - lastMouseX;
			windowPosition.v += mouseY - lastMouseY;
		}

		SpriteFill(windowSprite, 0, 0, 250 * .75, 2, PIXEL_SOLID, BG_CYAN | FG_CYAN);

		return 0;
	}

	int Exit()
	{
		wavMain->wavData->lpSecondaryBuffer->Stop();
		wavEat->wavData->lpSecondaryBuffer->Stop();
		wavDeath->wavData->lpSecondaryBuffer->Stop();

		exitProgram = true;

		return 0;
	}

	int Display() override
	{
		for(int i = 0; i < windowSprite->nWidth; i++)
			for (int j = 0; j < 2; j++)
			{
				windowSprite->SetColour(i, j, FG_CYAN | BG_CYAN);
				windowSprite->SetGlyph(i, j, PIXEL_SOLID);
			}

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 2), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 2), 1, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 0, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 0, BG_CYAN | FG_RED);

		windowSprite->SetGlyph((windowSprite->nWidth - 3), 1, PIXEL_THREEQUARTERS);
		windowSprite->SetColour((windowSprite->nWidth - 3), 1, BG_CYAN | FG_RED);


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

			srand(time(NULL));

			if (bBooted) return true;

			sprGameOver = new olcSprite(L"resources\\snakeGameOver.png.spr");

			wavMain		= new PcSound;
			wavDeath	= new PcSound;
			wavEat		= new PcSound;

			vPcSounds.push_back(wavMain);
			vPcSounds.push_back(wavDeath);
			vPcSounds.push_back(wavEat);

			wavMain->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\snakeMain.wav", false, true, false);
			wavDeath->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\snakeDie.wav", false, true, false);
			wavEat->wavData = SoundMacro->LoadSoundFileGetWave(L"resources\\snakeEat.wav", false, true, false);

			Reset();

			bBooted = true;
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