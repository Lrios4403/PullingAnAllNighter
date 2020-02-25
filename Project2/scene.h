#ifndef SCENE_H
#define SCENE_H

#include "olcConsoleGameEngine.h"

class Scene
{
protected:
	olcConsoleGameEngine* olcEngine;

public:
	virtual bool Initalize(olcConsoleGameEngine* e) = 0;
	virtual bool Switch(long id) = 0;
	virtual bool Update(float fElapsedTime, long* currentSceneIndex) = 0;
	virtual bool End() = 0;;

};

#endif