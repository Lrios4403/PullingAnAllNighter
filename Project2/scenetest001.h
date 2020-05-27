#ifndef SCENETEST001_H
#define SCENETEST001_H

#include "scene.h"
#include "consoleManager.h"
#include "computer.h"
#include "render.h"
#include "sound.h"

class SceneTest001 : public Scene
{
private:
	Render render;
	olcSprite* screenSprite;
	bool bCaptureMouse = true;
	bool bFocusToggle = false;
	bool bFlicker = false;
	bool bInComputer = false;
	float fElapsedTotal = 0;

	Computer computer;

public:
	bool Initalize(olcConsoleGameEngine* e) override
	{
		odprintf("Initalizing SCENE[Scene11]...");
		debugTabIndex++;

		this->olcEngine = e;

		render = Render();															//Set initalize the renderer
		render.Initalize(300, 100);
		render.map.Initalize("objects\\room.obj");


		render.map.rBounds.push_back(rectangle(-5.750000, 3.785412, 1.9, 3.714588));
		render.map.rBounds.push_back(rectangle(-5.750000, (1.070824 / 2) - .05 - 1, 1.459283, (2.046306 * 1.3) - .05+1));

		//render.map.rBounds.push_back({ {vec2d(-3,-3),vec2d(-3,3),vec2d(3,-3),vec2d(3,3)} });
		//render.map.rBounds[0].Reraster();

		render.map.rPlayerBouds.p[0] = vec2d(-1, -1);
		render.map.rPlayerBouds.p[1] = vec2d(-1, 1);
		render.map.rPlayerBouds.p[2] = vec2d(1, -1);
		render.map.rPlayerBouds.p[3] = vec2d(1, 1);

		rectangle r1(5, 5, 50, 50);
		rectangle r2(10, 10, 10, 10);
		r1.Reraster(); r2.Reraster();
		bool b = r1.bFastCollidsWith(r2);

		odprintf(b ? "true" : "false");

		computer = Computer();															//Initalize the computer
		computer.Initalize(olcEngine, 0, 0);
		computer.Update(e, 0, 0);

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
		}

		debugTabIndex--;
		return true;
	}

	bool Switch(long id) override
	{
		olcEngine->ConstructConsole(300, 200, 4, 4);

		render.bHighlightObject = false;
		render.bHighlightTriangle = false;
		render.Resize(300, 100);
		render.RenderStart();

		odprintf("SCENE Switched to SceneBasic!");

		return true;
	}

	bool Update(float fElapsedTime, long* currentSceneIndex) override
	{
		if (olcEngine->GetKey(VK_LEFT).bHeld)
		{
			render.map.vCamera.x -= fElapsedTime;
		}

		if (olcEngine->GetKey(VK_RIGHT).bHeld)
		{
			render.map.vCamera.x += fElapsedTime;
		}

		if (olcEngine->GetKey(VK_UP).bHeld)
		{
			render.map.vCamera.y -= fElapsedTime;
		}

		if (olcEngine->GetKey(VK_DOWN).bHeld)
		{
			render.map.vCamera.y += fElapsedTime;
		}

		unsigned long l = 0;
		for (auto r : render.map.rBounds)
		{
			odprintf("%lu %f %f", l, r.p[0].u, r.p[0].v);
			olcEngine->DrawLine((r.p[0].u + 10) * 10, (r.p[0].v + 10) * 10, (r.p[1].u + 10) * 10, (r.p[1].v + 10) * 10);
			olcEngine->DrawLine((r.p[3].u + 10) * 10, (r.p[3].v + 10) * 10, (r.p[1].u + 10) * 10, (r.p[1].v + 10) * 10);
			olcEngine->DrawLine((r.p[0].u + 10) * 10, (r.p[0].v + 10) * 10, (r.p[2].u + 10) * 10, (r.p[2].v + 10) * 10);
			olcEngine->DrawLine((r.p[3].u + 10) * 10, (r.p[3].v + 10) * 10, (r.p[2].u + 10) * 10, (r.p[2].v + 10) * 10);
		}

		odprintf("P %f %f", render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[0].u, render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[0].v);
		short col = 0;
		if (render.map.CheckCollision()) col = FG_RED;
		else col = FG_GREEN;
		olcEngine->DrawLine((render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[0].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[0].v + 10) * 10, (render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[1].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[1].v + 10) * 10, PIXEL_SOLID, col);
		olcEngine->DrawLine((render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[3].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[3].v + 10) * 10, (render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[1].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[1].v + 10) * 10, PIXEL_SOLID, col);
		olcEngine->DrawLine((render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[0].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[0].v + 10) * 10, (render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[2].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[2].v + 10) * 10, PIXEL_SOLID, col);
		olcEngine->DrawLine((render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[3].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[3].v + 10) * 10, (render.map.rPlayerBouds.offset.u + render.map.rPlayerBouds.p[2].u + 10) * 10, (render.map.rPlayerBouds.offset.v + render.map.rPlayerBouds.p[2].v + 10) * 10, PIXEL_SOLID, col);

		return true;
	}

	bool IntoComputer(bool s)
	{
		bInComputer = s;

		if (s)
		{
			olcEngine->ConstructConsole(300, 100, 4, 8);
		}
		else
		{
			olcEngine->ConstructConsole(300, 200, 4, 4);
		}

		return true;
	}

	bool End() override
	{

		return true;
	}
};

#endif