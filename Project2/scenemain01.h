#ifndef SCENE01_H
#define SCENE01_H

#include "scene.h"

#include "render.h"
#include "computer.h"
#include "sound.h"
#include "object.h"
#include "anticheat.h"
#include "debug.h"
#include "float.h"

class Scene01 : public Scene
{
private:
	vector<object*> objsHouse;						// The house object data
	object* objWindow;
	object* objWindowOpen;
	object* objWindowClose;
	olcSprite* sprHouse, * sprHouseDark;

	olcSprite* sprActionTab, * sprActionE;

	bool bWindowOpen = false;
	bool bWindowOpenAnimation = false;

	olcSprite meshSprite;							// The sprite for the mountain
	float fFanSpeed = 0.0f;							// The speed of the fan
	float fCurrentSpeed = 0.0f;						// The current fan speed
	float fElapsedTotal = 0.0f;						// Total elapsed time
	float fFps = 1000 / 24;							// The float for fps
	float fRelapses;
	bool captureMouse = true;						// If the mouse is captured
	bool renderCpu = false;
	bool lightsOn = true;							// If the lights are on
	bool bInComputer = true;

	bool bMovingTowardsComputer = false;			// The animation to see if we are moving towards the pc
	bool bIntroAnimation = false;					//The identifiers for the intro animations
	int iIntroAnimation = 0;
	vector<vec3d> vv3IntroAnimations = {
		{-4.933629, 0.500000, -4.893097},
		{3.851461, 0.500000, -0.464234},
		{-0.228252, 0.500000, 2.358223},
		{1.580737, 0.500000, 5.671022},
		{-1.720015, 0.500000, 5.671022},
		{-3.49799991, -0.365 + 0.05 - 0.025, 7.28939152},
		{ -4.3325f, -0.365 + 0.05 - 0.025, 5.6125}

	};
	vector<vec2d> vv2IntroAnimations =
	{
		{0.210000, -0.750000},
		{0.300000, 0.940001},
		{0.415000, -1.099999},
		{0.570000, -0.054999},
		{0.500000, -0.024999},
		{0.20, 2.34642911},
		{0.0, (PI / 2.0f)},
	};
	vector<bool> vbIntroAnimationExp =
	{
		true,
		true,
		true,
		true,
		true,
		true,
		true,
	};
	vector<olcSprite*> vspriteArray =
	{
		new olcSprite(L"resources\\tutorial msg1.png.spr"),
		new olcSprite(L"resources\\tutorial msg2.png.spr"),
		new olcSprite(L"resources\\tutorial msg3.png.spr"),
		new olcSprite(L"resources\\tutorial msg4.png.spr"),
		new olcSprite(L"resources\\tutorial msg5.png.spr"),
		new olcSprite(L"resources\\tutorial msg6.png.spr"),
		new olcSprite(L"resources\\tutorial msg7.png.spr"),
	};

	Render render;
	Computer computer;

	olcSprite* titleSprite;

	vector<rectangle> rectInvisBarrier;				// Setup the impassable walls

	WaveData* wavHighPitch;
	WaveData* wavAmbience;

	bool renderscreen = true;



public:
	bool Initalize(olcConsoleGameEngine* olce) override
	{
		odprintf("Initalizing SCENE[main game]...");
		debugTabIndex++;

		olcEngine = olce;
		render = Render();														//Set initalize the renderer
		render.Initalize(olcEngine->ScreenWidth(), olcEngine->ScreenHeight());
		render.fFPS = 1000.0f / 120.0f;
		computer = Computer();													//Initalize the computer
		computer.Initalize(olcEngine, 0, 0);

		odprintf("%i", sizeof(Render));

		if (SoundMacro->LoadSoundFile(L"sound01.wav", true, true, true) == 0)
		{
			wavHighPitch = SoundMacro->vSounds[SoundMacro->vSounds.size() - 1];
		}


		if (SoundMacro->LoadSoundFile(L"ambience.wav", true, true, true) == 0)
		{
			wavAmbience = SoundMacro->vSounds[SoundMacro->vSounds.size() - 1];
			//wavAmbience->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
		}

		sprHouse				= new olcSprite(L"resources\\house text.png.spr");
		sprHouseDark			= new olcSprite(L"resources\\house text dark.png.spr");

		objsHouse				= LoadFromObjectFile("objects\\room.obj", sprHouse, false);
		objWindowOpen			= new object("objects\\window open.obj", false);
		objWindowClose			= new object("objects\\window closed.obj", false);

		sprActionTab			= new olcSprite(L"resources\\info tab.png.spr");
		sprActionE				= new olcSprite(L"resources\\info e.png.spr");

		titleSprite = new olcSprite(L"resources\\titlescreen.png.spr");

		for (int i = 0; i < objsHouse.size(); i++)
		{
			if (wcscmp(objsHouse[i]->object_mesh.name.c_str(), L"Window_1_Plane.004"))
			{
				objWindow = objsHouse[i];
			}

			render.AddObject(objsHouse[i]);
		}

		objsHouse[9]->sprite = computer.GetScreenSprite();

		render.vCamera.z = -0.1;
		render.vCamera.x = 0.125;

		//Set the basic clear boxes
		rectInvisBarrier.push_back(rectangle());

		rectInvisBarrier[rectInvisBarrier.size() - 1].p0 = { 3.661183, 0.0,  -5.750000 };
		rectInvisBarrier[rectInvisBarrier.size() - 1].p1 = { 7.500000, 0.0, -3.841818 };

		ToggleLights(true);

		CenterWindow();

		//Set the console window to disable resizing
#ifdef	OLC_H
		SetWindowLong(GetConsoleWindow(), GWL_STYLE, GetWindowLong(GetConsoleWindow(), GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

#endif

		render.vCamera = { -4.933629, 0.500000, -4.893097 };

		debugTabIndex--;
		odprintf("Initalized the  SCENE[main game]!");

		Sleep(1);

		return true;
	}

	bool Switch(long id) override
	{
		render.RenderStart();

		return true;
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

	void ToggleLights(bool l)
	{
		lightsOn = l;

		for (auto o : objsHouse)
		{
			if (lightsOn)
				o->sprite = sprHouse;
			else
				o->sprite = sprHouseDark;
		}

		objsHouse[9]->sprite = computer.GetScreenSprite();

	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		float fSleepTime = fFps - fElapsedTime;

		//odprintf("%f %f %f", fFps, fElapsedTime, fSleepTime);

		float fMoveSpeed = 8.0f;

		vec3d vCameraLast = render.vCamera;

		//		fCurrentSpeed += 1*fElapsedTotal;
		//		if (fCurrentSpeed > PI * 0.05f) fCurrentSpeed = PI * 0.05f;
		//		fFanSpeed += fCurrentSpeed;

		//		render.objects[12]->setRotation(0, fFanSpeed, 0);

		if (olcEngine->GetKey(VK_UP).bHeld)
			render.vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards

		if (olcEngine->GetKey(VK_DOWN).bHeld)
			render.vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards

		if (olcEngine->GetKey(VK_SHIFT).bHeld)
			fMoveSpeed *= 1.75f;

		// Dont use these two in FPS mode, it is confusing :P
		if (olcEngine->GetKey(VK_LEFT).bHeld)
			render.vCamera.x -= 1.0f * fElapsedTime;	// Travel Along X-Axis

		if (olcEngine->GetKey(VK_RIGHT).bHeld)
			render.vCamera.x += 1.0f * fElapsedTime;	// Travel Along X-Axis
		///////

		vec3d vForward = Vector_Mul(render.vLookDir, fMoveSpeed * fElapsedTime);

		// Standard FPS Control scheme, but turn instead of strafe
		if (olcEngine->GetKey(L'W').bHeld)
			render.vCamera = Vector_Add(render.vCamera, vForward);

		if (olcEngine->GetKey(L'S').bHeld)
			render.vCamera = Vector_Sub(render.vCamera, vForward);

		if (olcEngine->GetKey(L'A').bHeld)
			render.fYaw -= 2.0f * fElapsedTime;

		if (olcEngine->GetKey(L'D').bHeld)
			render.fYaw += 2.0f * fElapsedTime;

		if (olcEngine->GetKey(L'E').bPressed)
		{
			bWindowOpen = !bWindowOpen;
			bWindowOpenAnimation = true;
			//soundManager.vSounds[0]->lpSecondaryBuffer->Play(0, 0, 0);
		}

		if (olcEngine->GetKey(VK_SPACE).bPressed)
		{
			iIntroAnimation++;
		}

		//CheckCameraBounds();

		//Use the mouse for camera rotation
		if (olcEngine->GetKey(27).bPressed)
		{
			HWND h_console = GetConsoleWindow();
			RECT size; GetWindowRect(h_console, &size);
			int width = size.right - size.left;
			int height = size.bottom - size.top;
			POINT cursorPos;
			captureMouse = !captureMouse;
			SetCursorPos((width / 2) + size.left, (height / 2) + size.top);
		}

		bool bCameraRotMove = false;

		if (bIntroAnimation && iIntroAnimation < 7)
		{
			float fLookSpeed = fElapsedTime * 8.0f;

			vec3d vDiff = Vector_Sub(vv3IntroAnimations[iIntroAnimation], render.vCamera);																//Get the difference from the camera to the computer
			vDiff = Vector_Mul(vDiff, fLookSpeed);

			float fYawDiff = (render.fYaw - vv2IntroAnimations[iIntroAnimation].v) * fLookSpeed;														//Get the pitch/yaw difference
			float fPitchDiff = (render.fPitch - vv2IntroAnimations[iIntroAnimation].u) * fLookSpeed;

			//odprintf("%i %f %f %f", iIntroAnimation, vDiff.x, vDiff.y, vDiff.z);

			//odprintf("%f %f %f", render.vCamera.x, render.vCamera.y, render.vCamera.z);

			render.fYaw -= fYawDiff;
			render.fPitch -= fPitchDiff;

			render.vCamera = Vector_Add(render.vCamera, vDiff);

			olcEngine->DrawSprite(0, 0, render.screenSprite);

			olcEngine->DrawSprite(0, 0, vspriteArray[iIntroAnimation]);

		}
		else
		{

			if (render.vCamera.x >= 5.45f && render.vCamera.z < -1.25f && render.vCamera.z > -5.9f)
			{

			}

			if (!renderscreen)
			{
				if (captureMouse)
				{
					HWND h_console = GetConsoleWindow();
					RECT size; GetWindowRect(h_console, &size);
					int width = size.right - size.left;
					int height = size.bottom - size.top;
					POINT cursorPos;
					GetCursorPos(&cursorPos);

					float yawOffset = (float)((cursorPos.x - ((width / 2) + size.left)) * 0.005);
					float pitchOffset = (float)((cursorPos.y - ((height / 2) + size.top)) * 0.005);

					//if (yawOffset != 0 || pitchOffset != 0) render.bMustRender = true;

					render.fYaw += yawOffset;
					render.fPitch += pitchOffset;

					if (render.fYaw > TWO_PI) render.fYaw -= TWO_PI;
					if (render.fYaw < -TWO_PI) render.fYaw += TWO_PI;
					if (render.fPitch > HALF_PI) render.fPitch = HALF_PI - 0.001f;
					if (render.fPitch < -HALF_PI) render.fPitch = -HALF_PI + 0.001f;

					SetCursorPos((width / 2) + size.left, (height / 2) + size.top);

					ShowCursor(false);

				}
				else
				{
					ShowCursor(true);
				}
			}

			//Make sure the camera is where it is supposed to be
			if (!bMovingTowardsComputer)																				//If the camera is in the computer-looking-at anamation
			{
				render.vCamera.y = 0.5f;

				if (render.vCamera.x > 5.75f) render.vCamera.x = 5.75f;
				if (render.vCamera.x < -5.75f) render.vCamera.x = -5.75f;
				if (render.vCamera.z > 7.5f) render.vCamera.z = 7.5f;
				if (render.vCamera.z < -7.5f) render.vCamera.z = -7.5f;

				for (int i = 0; i < rectInvisBarrier.size(); i++)
				{
					rectInvisBarrier[i].GetOrganizedFloats();

				}
			}

			if (olcEngine->GetKey(VK_TAB).bPressed)
			{
				if (bInComputer == true)
				{
					renderscreen = false;
					bInComputer = false;
					bMovingTowardsComputer = false;

					HWND h_console = GetConsoleWindow();
					RECT size; GetWindowRect(h_console, &size);
					int width = size.right - size.left;
					int height = size.bottom - size.top;
					POINT cursorPos;
					SetCursorPos((width / 2) + size.left, (height / 2) + size.top);

					render.fYaw = (PI / 2.0f);
					render.fPitch = 0.6f;

					render.vCamera =
					{
						-3.25f,
						0.500000000,
						5.60197401,
						1.0f
					};

					captureMouse = true;

					render.WaitForNextFrame();
					render.WaitForNextFrame();

				}
				else
				{
					this->bMovingTowardsComputer = true;
					this->bInComputer = true;
				}

			}

			if (bMovingTowardsComputer)
			{
				float fLookSpeed = fElapsedTime * 17.5f;
				vec3d vMoveTo =
				{
					-4.3325f,
					-0.365 + 0.05 - 0.025,
					5.6125,
					1.0f
				};

				vec3d vDiff = Vector_Sub(vMoveTo, render.vCamera);																//Get the difference from the camera to the computer
				vDiff = Vector_Mul(vDiff, fLookSpeed);

				float fYawDiff = (render.fYaw - (PI / 2.0f)) * fLookSpeed;														//Get the pitch/yaw difference
				float fPitchDiff = (render.fPitch) * fLookSpeed;

				//odprintf("%f %f %f", render.vCamera.x, render.vCamera.y, render.vCamera.z);

				render.vCamera = Vector_Add(render.vCamera, vDiff);
				render.fYaw -= fYawDiff;
				render.fPitch -= fPitchDiff;

				if (Float_Distance(render.vCamera, vMoveTo) < 0.01f)
				{
					bMovingTowardsComputer = false;
					renderscreen = true;
				}

				captureMouse = false;
			}

			if (bWindowOpenAnimation)
			{
				triangle* tri1 = new triangle();
				triangle* tri2 = new triangle();
				triangle tri3 = triangle();

				for (int i = 0; i < objWindow->object_mesh.tris.size(); i++)
				{

					tri1 = &objWindow->object_mesh.tris[i];

					if (bWindowOpen) tri2 = &objWindowOpen->object_mesh.tris[i];
					else			tri2 = &objWindowClose->object_mesh.tris[i];

					tri3 = *tri1;

					for (int j = 0; j < 3; j++)
					{
						vec3d vDiff = Vector_Sub(tri1->p[j], tri2->p[j]);
						vDiff = Vector_Mul(vDiff, fElapsedTime * 16.0f);

						tri3.p[j] = Vector_Sub(tri1->p[j], vDiff);
						tri3.t[j] = tri1->t[j];
					}

					objWindow->object_mesh.tris[i] = tri3;

				}

			}

			if (renderscreen)
			{
				computer.Update(olcEngine, fElapsedTime, fElapsedTotal);
				olcEngine->DrawSprite(0, 0, computer.GetScreenSprite());
			}
			else
			{
				olcEngine->DrawSprite(0, 0, render.screenSprite);

				odprintf("%f %f %f", render.vCamera.x, render.vCamera.y, render.vCamera.z);

				if (render.vCamera.x <= -2.278418f && render.vCamera.x >= -5.750000f)
					if (render.vCamera.z >= 3.736802f && render.vCamera.z <= 7.475547f)
					{
						//-3.651363 0.500000 5.594613
						float x1 = render.vCamera.x, x2 = -3.25f;
						float y1 = render.vCamera.z, y2 = 5.60197401;
						float diff = powf(x2 - x1, 2) + powf(y2 - y1, 2);
						diff = 1 - sqrtf(diff);

						float r = (float)(rand()%100)/100.0f;

						odprintf("%f %f ", diff, r);

						if(diff > r)
							olcEngine->DrawSprite((olcEngine->ScreenWidth() - sprActionTab->nWidth) / 2, olcEngine->ScreenHeight() - (sprActionTab->nHeight * 1.5), sprActionTab);
					}

				//-4.018768 0.500000 0.561126

				if (render.vCamera.x <= -4.018768+0.5f && render.vCamera.x >= -4.018768 -0.5f)
					if (render.vCamera.z >= 0.561126 - 0.5f && render.vCamera.z <= 0.561126 + 0.5f)
					{
						olcEngine->DrawSprite((olcEngine->ScreenWidth() - sprActionE->nWidth) / 2, olcEngine->ScreenHeight() - (sprActionE->nHeight * 1.5), sprActionE);
					}
			}


		}


		fElapsedTotal += fElapsedTime;

		return true;
	}

	bool End() override
	{


		return true;
	}

};

#endif