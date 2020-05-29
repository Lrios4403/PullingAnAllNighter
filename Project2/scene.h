#ifndef SCENE_H
#define SCENE_H

#include "olcConsoleGameEngine.h"

long sceneIndex = 0;

class Scene
{
protected:
	olcConsoleGameEngine* olcEngine;

public:
	float fCreateTime = 0.0f;

	virtual bool Initalize(olcConsoleGameEngine* e) = 0;
	virtual bool Switch(long id) = 0;
	virtual bool Update(float fElapsedTime, long* currentSceneIndex) = 0;
	virtual bool End() = 0;

	void Create(olcConsoleGameEngine* e)
	{
		auto tp1 = std::chrono::system_clock::now();
		Initalize(e);
		auto tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		fCreateTime = elapsedTime.count();
	}

};

#endif