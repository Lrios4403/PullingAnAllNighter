#ifndef TD_MAP_H
#define TD_MAP_H

#include <iostream>
#include <vector>
#include <string>

#include "object.h"
#include "cyclone/cyclone.h"
#include "cyclone/interpreter.h"

class TDMap
{
public:
	std::vector<object*> objects;
	std::vector<vec3d*> lights;

	vec3d vCamera;							// Location of camera in world space
	vec3d vLastCamera;

	vector<rectangle> rBounds;

	rectangle rPlayerBouds;

	TDMap()
	{};

	~TDMap()
	{
		objects.clear();
		objects.shrink_to_fit();
		lights.clear();
		lights.shrink_to_fit();

	};

	int Initalize(std::string objpath)
	{
		odprintf("Initalizing TDMap instance [%p]...!\n", this);
		debugTabIndex++;

		odprintf("Loading object file...");
		objects = LoadFromObjectFile(objpath, new olcSprite());

		odprintf("Initalized TDMap instance!");
		debugTabIndex--;
		return 0;
	}

	bool CheckCollision()
	{
		for (size_t t = 0; t < rBounds.size(); t++)
		{
			if (rBounds[t].bFastCollidsWith(rPlayerBouds)) return true;
		}

		return false;
	}

	int Update(float fElapsedTime)
	{
		rPlayerBouds.offset.u = vCamera.x;
		if (CheckCollision()) rPlayerBouds.offset.u = vLastCamera.x;
		rPlayerBouds.offset.v = vCamera.z;
		if (CheckCollision()) rPlayerBouds.offset.v = vLastCamera.z;

		vCamera.x = rPlayerBouds.offset.u;
		vCamera.z = rPlayerBouds.offset.v;

		vLastCamera = vCamera;

		return 0;
	}

	int AddObject(std::vector<object*> objs)
	{
		objects.insert(objects.end(), objs.begin(), objs.end());

		return 0;
	}

};

#endif