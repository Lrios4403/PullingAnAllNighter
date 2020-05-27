#ifndef SCENE11_H
#define SCENE11_H

#include "scene.h"
#include "consoleManager.h"
#include "computer.h"
#include "render.h"
#include "sound.h"

class Scene11 : public Scene
{
private:
	Render render;
	olcSprite* sprActionTab, * sprActionE;
	olcSprite* screenSprite;
	bool bCaptureMouse				= true;
	bool bFocusToggle				= false;
	bool bFlicker					= false;
	bool bInComputer				= false;
	float fElapsedTotal				= 0;
	float fCurrentAmbienceVolume	= 2.0f;

	vector<olcSprite*>	vspriteArray;

	Computer computer;
	rectangle rComputer;

	vector<WaveData*>	wdRadio;
	WaveData*			wdCurrentRadio			= nullptr;
	size_t				stRadio;
	rectangle			rRadio;
	WaveData*			wavHighPitch;
	WaveData*			wavAmbience;

	object* objWindowOpen, * objWindowClosed, * objWindowMain;
	rectangle			rWindow;
	WaveData*			wavWindow;
	float				fWavWindow				= 0;
	bool				bWindowOpen				= false;
	bool				bWindowOpenAnimation	= false;

	vector<object*>		objsWhitefacePos;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[Scene11]...");
		debugTabIndex++;

		this->olcEngine = e;

		odprintf("Loading the renderer...");
		render = Render();															//Set initalize the renderer
		render.Initalize(300, 100);
		render.map.Initalize("objects\\room.obj");
		odprintf("Adding whiteface...");
		render.map.AddObject(LoadFromObjectFile("objects\\whiteface.obj", true));
		render.map.objects[render.map.objects.size() - 1]->visible = false;

		olcSprite* tempspr = new olcSprite();
		tempspr->Load(L"resources\\whiteface.png.spr");

		vector<object*> objs	= LoadFromObjectFile("objects\\window open.obj", true);
		objWindowOpen			= objs[0];
		objs					= LoadFromObjectFile("objects\\window closed.obj", true);
		objWindowClosed			= objs[0];
		wavWindow				= SoundMacro->LoadSoundFileGetWave(L"resources\\window.wav", false, true, false);

		vspriteArray = 
		{
			new olcSprite(L"resources\\tutorial msg1.png.spr"),
			new olcSprite(L"resources\\tutorial msg2.png.spr"),
			new olcSprite(L"resources\\tutorial msg3.png.spr"),
			new olcSprite(L"resources\\tutorial msg4.png.spr"),
			new olcSprite(L"resources\\tutorial msg5.png.spr"),
			new olcSprite(L"resources\\tutorial msg6.png.spr"),
			new olcSprite(L"resources\\tutorial msg7.png.spr"),
		};

		sprActionTab	= new olcSprite(L"resources\\info tab.png.spr");
		sprActionE		= new olcSprite(L"resources\\info e.png.spr");

		render.map.rBounds.push_back(rectangle(-5.750000, 3.785412, 1.9, 3.714588));
		render.map.rBounds.push_back(rectangle(-5.750000, -0.835891-.1, 5.750000 - 4.257414, 3.054524 + 0.835891+.1));
		render.map.rBounds.push_back(rectangle(-2.360016, 6.108675, 4.552565, 1.452665));
		render.map.rBounds.push_back(rectangle(2.105414, 0.611446, 5.750000 - 2.105414, 7.500000 - 0.611446));
		render.map.rBounds.push_back(rectangle(5.346011, -5.948021, 5.748881 - 5.346011, 5.948021 - 1.000364));

		rRadio		= rectangle(-4.239238, -0.951322, 4.239238 - 3.168512, 1.191415 + 0.951322);
		rComputer	= rectangle(-3.849858, 4.790962, 3.849858 - 2.641146, 6.488769 - 4.790962);
		rWindow		= rectangle(4.508488, -6.134451, 5.750000 - 4.508488, 6.134451 - 1.144568);

		render.map.rPlayerBouds.p[0] = vec2d(-.0001, -.0001);
		render.map.rPlayerBouds.p[1] = vec2d(-.0001, .0001);
		render.map.rPlayerBouds.p[2] = vec2d(.0001, -.0001);
		render.map.rPlayerBouds.p[3] = vec2d(.0001, .0001);

		computer = Computer();															//Initalize the computer
		computer.Initalize(olcEngine, 0, 0);
		computer.Update(e, 1, 1);
		
		for (size_t l = 0; l < render.map.objects.size(); l++)
		{
			auto foundpos = render.map.objects[l]->object_mesh.name.find(wstring(L"Monitor_Screen_Cube"));
			if (foundpos != std::string::npos)
			{
				for (size_t l2 = 0; l2 < render.map.objects[l]->object_mesh.tris.size(); l2++)
				{
					olcSprite* spr = computer.GetScreenSprite();
					object* obj = render.map.objects[l];
					triangle3d* tri = &obj->object_mesh.tris[l2];
					tri->spritePtr = spr;

					tri->t[0].v = 1 - tri->t[0].v;
					tri->t[1].v = 1 - tri->t[1].v;
					tri->t[2].v = 1 - tri->t[2].v;

				}
			}

			foundpos = render.map.objects[l]->object_mesh.name.find(wstring(L"Window_2"));
			if (foundpos != std::string::npos)
			{
				objWindowMain = render.map.objects[l];
			}

			foundpos = render.map.objects[l]->object_mesh.name.find(wstring(L"whiteface"));
			if (foundpos != std::string::npos)
			{
				objsWhitefacePos.push_back(render.map.objects[l]);
				render.map.objects[l]->visible = false;
			}
		}

		for (int i = 0; i < render.map.objects[render.map.objects.size() - 1]->object_mesh.tris.size(); i++)
		{

			render.map.objects[render.map.objects.size()-1]->object_mesh.tris[i].p[0] = objsWhitefacePos[1]->object_mesh.tris[i].p[0];
			render.map.objects[render.map.objects.size()-1]->object_mesh.tris[i].p[1] = objsWhitefacePos[1]->object_mesh.tris[i].p[1];
			render.map.objects[render.map.objects.size()-1]->object_mesh.tris[i].p[2] = objsWhitefacePos[1]->object_mesh.tris[i].p[2];
		}

		odprintf("Allocating sprite memory...");
		screenSprite = new olcSprite(300, 100);

		if (SoundMacro->LoadSoundFile(L"ambeince.wav", false, true, false) == 0)
		{
			wavAmbience = SoundMacro->vSounds[SoundMacro->vSounds.size() - 1];
			wavAmbience->SetVolume(fCurrentAmbienceVolume, 100);
		}

		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc1.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc2.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc3.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc4.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc5.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\radiomc6.wav", false, true, false));
		wdRadio.push_back(SoundMacro->LoadSoundFileGetWave(L"resources\\penis.wav", false, true, false));

		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(300, 100, 4, 8);

		render.bHighlightObject = false;
		render.bHighlightTriangle = false;
		render.Resize(300, 100);
		render.RenderStart();
		wavAmbience->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

		odprintf("SCENE Switched to SceneBasic!");

		return true;
	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		fElapsedTotal += fElapsedTime;

		//Misc.
		if (fWavWindow < .5)
		{
			fWavWindow += fElapsedTime;

			if (fWavWindow > .5)
			{
				wavWindow->lpSecondaryBuffer->Stop();
			}
		}

		//Handle animations
		if (bWindowOpenAnimation)
		{
			triangle3d* tri1 = new triangle3d();
			triangle3d* tri2 = new triangle3d();
			triangle3d tri3 = triangle3d();

			for (int i = 0; i < objWindowMain->object_mesh.tris.size(); i++)
			{

				tri1 = &objWindowMain->object_mesh.tris[i];

				if (bWindowOpen) tri2 = &objWindowOpen->object_mesh.tris[i];
				else			tri2 = &objWindowClosed->object_mesh.tris[i];

				tri3 = *tri1;

				for (int j = 0; j < 3; j++)
				{
					vec3d vDiff = Vector_Sub(tri1->p[j], tri2->p[j]);
					vDiff = Vector_Mul(vDiff, fElapsedTime * 16.0f);

					tri3.p[j] = Vector_Sub(tri1->p[j], vDiff);
					tri3.t[j] = tri1->t[j];
				}

				objWindowMain->object_mesh.tris[i] = tri3;

			}
		}

		//Handle movements and render
		if (bInComputer)
		{
#pragma region HandleComputerRender
			computer.Update(olcEngine, fElapsedTime, fElapsedTotal);
			olcEngine->DrawSprite(0, 0, computer.GetScreenSprite());
			if (olcEngine->GetKey(VK_TAB).bPressed)
				IntoComputer(false);

			for (size_t t = 0; t < vPcSounds.size(); t++)
			{
				float fv = vPcSounds[t]->fSoundMin;

				vPcSounds[t]->wavData->SetVolume(fv, vPcSounds[t]->fSoundMax);

			}

#pragma endregion
		}
		else
		{
#pragma region HandleRenderMovement
			vec3d vForward = Vector_Mul(render.vLookDir, fElapsedTime * 10);

			// Standard FPS Control scheme, but turn instead of strafe
			if (olcEngine->GetKey(L'W').bHeld)
				render.map.vCamera = Vector_Add(render.map.vCamera, vForward);

			if (olcEngine->GetKey(L'S').bHeld)
				render.map.vCamera = Vector_Sub(render.map.vCamera, vForward);

			if (olcEngine->GetKey(L'A').bHeld)
				render.fYaw -= 2.0f * fElapsedTime;

			if (olcEngine->GetKey(L'D').bHeld)
				render.fYaw += 2.0f * fElapsedTime;

			//if (render.map.vCamera.x > 5.75f) render.map.vCamera.x = 5.75f;
			//if (render.map.vCamera.x < -5.75f) render.map.vCamera.x = -5.75f;
			//if (render.map.vCamera.z > 7.5f) render.map.vCamera.z = 7.5f;
			//if (render.map.vCamera.z < -7.5f) render.map.vCamera.z = -7.5f;

			if (olcEngine->GetKey(27).bPressed)
			{
				bCaptureMouse = !bCaptureMouse;
			}

			if (bFocusToggle != olcEngine->IsFocused())
			{
				bFocusToggle = olcEngine->IsFocused();

				bCaptureMouse = bFocusToggle;
			}

			if (bCaptureMouse)
			{
				HWND h_console = GetConsoleWindow();
				RECT size; GetWindowRect(h_console, &size);
				int width = size.right - size.left;
				int height = size.bottom - size.top;
				POINT cursorPos;
				GetCursorPos(&cursorPos);

				float yawOffset = (float)((cursorPos.x - ((width / 2) + size.left)) * 0.005);
				float pitchOffset = (float)((cursorPos.y - ((height / 2) + size.top)) * 0.005);

				render.fYaw += yawOffset;
				render.fPitch += pitchOffset;

				if (render.fYaw > TWO_PI) render.fYaw -= TWO_PI;
				if (render.fYaw < -TWO_PI) render.fYaw += TWO_PI;
				if (render.fPitch > HALF_PI) render.fPitch = HALF_PI - 0.001f;
				if (render.fPitch < -HALF_PI) render.fPitch = -HALF_PI + 0.001f;

				SetCursorPos((width / 2) + size.left, (height / 2) + size.top);
			}

			render.map.vCamera.y = 0.5;

			//Handle the pc sounds
			for (size_t t = 0; t < vPcSounds.size(); t++)
			{
				float fv = vPcSounds[t]->fSoundMin;
				float fm = 6.631576-(Float_Distance(render.map.vCamera, vec3d(-3.783792, 0.500000, 5.653103)));
				fv *= fm;

				vPcSounds[t]->wavData->SetVolume(fv, vPcSounds[t]->fSoundMax);

			}

			//render.CopySprite(screenSprite);
			//olcEngine->DrawSprite(0, 0, screenSprite);
			render.DrawSprite(olcEngine);

			if (render.map.rPlayerBouds.bFastCollidsWith(rComputer))
			{
				olcEngine->DrawSprite((olcEngine->ScreenWidth() - sprActionTab->nWidth) / 2, olcEngine->ScreenHeight() - (sprActionTab->nHeight * 1.5), sprActionTab);
				if (olcEngine->GetKey(VK_TAB).bPressed)
					IntoComputer(true);
			}

			if (render.map.rPlayerBouds.bFastCollidsWith(rRadio))
			{
				olcEngine->DrawSprite((olcEngine->ScreenWidth() - sprActionE->nWidth) / 2, olcEngine->ScreenHeight() - (sprActionE->nHeight * 1.5), sprActionE);
				
				if (olcEngine->GetKey(L'E').bPressed)
				{
					for (auto w : wdRadio)
					{
						w->lpSecondaryBuffer->Stop();
						w->lpSecondaryBuffer->SetCurrentPosition(0);
					}

					if (stRadio < wdRadio.size())
					{
						wdRadio[stRadio]->lpSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
						wdCurrentRadio = wdRadio[stRadio];
						stRadio++;
					}
					else if (stRadio == wdRadio.size())
					{
						wdCurrentRadio = nullptr;
						stRadio = 0;
					}
				}
			}

			if (render.map.rPlayerBouds.bFastCollidsWith(rWindow))
			{
				olcEngine->DrawSprite((olcEngine->ScreenWidth() - sprActionE->nWidth) / 2, olcEngine->ScreenHeight() - (sprActionE->nHeight * 1.5), sprActionE);

				if (olcEngine->GetKey(L'E').bPressed)
				{
					wavWindow->lpSecondaryBuffer->SetCurrentPosition(0);
					wavWindow->lpSecondaryBuffer->Play(0, 0, 0);
					fWavWindow = 0;
					bWindowOpenAnimation = true;
					bWindowOpen = !bWindowOpen;
				}
			}

#pragma endregion

		}

		//Handle the ambience sound
		wavAmbience->SetVolume(fCurrentAmbienceVolume, 100);
		fCurrentAmbienceVolume -= fElapsedTime * .05;
		if (fCurrentAmbienceVolume < 1.f)	fCurrentAmbienceVolume = 1.f;
		if (wdCurrentRadio != nullptr)		wdCurrentRadio->SetVolume(12-Vector_Length(render.map.vCamera - vec3d(-4.257021,0.500000,0.153504)), 12);

		return true;
	}

	bool IntoComputer(bool s)
	{
		bInComputer = s;

		if (s)
		{
			render.RenderStop();
		}
		else
		{
			render.RenderStart();
		}

		return true;
	}

	bool End() override
	{

		return true;
	}
};

#endif