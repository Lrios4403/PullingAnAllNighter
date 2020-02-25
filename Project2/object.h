#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <vector>

#include "olcConsoleGameEngine.h"
#include "objecth.h"
#include "tdmath.h"
#include "debug.h"
#include "float.h"

using namespace std;

struct object
{
	mesh object_mesh;
	olcSprite* sprite;
	mat4x4 position, rotation;
	float rx, ry, rz;
	float lighting_range[2] = { 0.0f, 1.0f };

	object()
	{
		odprintf("Created Object STRUCT!");
		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();
	}

	object(mesh m)
	{
		odprintf("Created Object STRUCT!");
		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();

		object_mesh = m;

	}

	object(string file, bool text)
	{
		odprintf("Created Object STRUCT [%s]!", file.c_str());

		object_mesh.LoadFromObjectFile(file, text);

		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();
	}

	object(string file, bool text, float x, float y, float z)
	{
		odprintf("Created Object STRUCT [%s, %f, %f, %f]!", file.c_str(), x, y, z);

		object_mesh.LoadFromObjectFile(file, text);

		position = Matrix_MakeIdentity();
		rotation = Matrix_MakeIdentity();

		setPosition(x, y, z);
	}

	bool clear()
	{
		object_mesh.tris.clear();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				position.m[i][j] = 1.0f;

		return true;

	}

	void fromMesh(mesh m)
	{
		object_mesh = m;
	}

	bool setPosition(float x, float y, float z)
	{
		position = Matrix_MakeTranslation(x, y, z);

		return true;
	}

	bool setPosition(vec3d p)
	{
		return setPosition(p.x, p.y, p.z);
	}

	bool setRotation(float x, float y, float z)
	{
		mat4x4  rotationx, rotationy, rotationz;

		rx = x;
		ry = y;
		rz = z;

		rotationx = Matrix_MakeRotationX(x);
		rotationy = Matrix_MakeRotationY(y);
		rotationz = Matrix_MakeRotationZ(z);

		rotation = Matrix_MakeIdentity();
		rotation = Matrix_MultiplyMatrix(rotationx, rotationy); // Transform by rotation
		rotation = Matrix_MultiplyMatrix(rotation, rotationz); // Transform by rotation

		return true;
	}
};

vector<object*> LoadFromObjectFile(string sFilename, olcSprite* sprite, bool bHasTexture = false)
{
	vector<object*> objects;
	mesh tempmesh;
	vector<triangle> tris;
	wstring name;

	ifstream f(sFilename);
	if (!f.is_open())
		return vector<object*>();

	// Local cache of verts
	vector<vec3d> verts;
	vector<vec2d> texs;

	int i = 0;

	char line[128];
	while (!f.eof())
	{
		f.getline(line, 128);

		strstream s;
		s << line;

		char junk;

		i++;
		
		if (line[0] == 'o')
		{
			if (objects.size() == 0 && name.length() == 0)
			{
				wstring stringline(&line[0], &line[128]);									//Get the current name of the object
				name = stringline.substr(2, stringline.length());

			}
			else
			{
			//	//convert all values into minifloat
			//	for (int i = 0; i < tris.size(); i++)
			//	{
			//		for (int j = 0; j < 3; j++)
			//		{
			//			odprintf("%f %f ", tris[i].p[j].x , float8_to_float(float_to_float8(tris[i].p[j].x)));

			//			//tris[i].p[j].x = float8_to_float(float_to_float8(tris[i].p[j].x));
			//		}
			//	}

				tempmesh = mesh{ tris, {vec3d(), vec3d()}, name };							//Set the mesh to 
				tempmesh.GetBounds();														//Get the map bounds
				objects.push_back(new object(tempmesh));									//Add the mesh to the list
				objects[objects.size() - 1]->sprite = sprite;								//Set the sprite
				tris.clear();																//Clear the tirangle list

				wstring stringline(&line[0], &line[128]);									//Get the current name of the object
				name = stringline.substr(2, stringline.length());

			}

		}

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				vec2d v;
				s >> junk >> junk >> v.u >> v.v;
				// A little hack for the spyro texture
				//v.u = 1.0f - v.u;
				//v.v = 1.0f - v.v;
				texs.push_back(v);
			}
			else
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
		}

		if (!bHasTexture)
		{
			if (line[0] == 'f')
			{
				if (strchr(line, '/') != NULL)
				{
					goto gotobHasTexture;
				}

				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		else
		{
		gotobHasTexture:

			if (line[0] == 'f')
			{
				s >> junk;

				string tokens[6];
				int nTokenCount = -1;


				while (!s.eof())
				{
					char c = s.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();


				tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
					texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

			}

		}
	}

	//We need to add the ending object to the list
	tempmesh = mesh{ tris, {vec3d(), vec3d()}, name };							//Set the mesh to 
	tempmesh.GetBounds();														//Get the map bounds
	objects.push_back(new object(tempmesh));									//Add the mesh to the list
	objects[objects.size() - 1]->sprite = sprite;								//Set the sprite


	return objects;
}

#endif