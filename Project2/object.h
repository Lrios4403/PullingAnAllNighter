#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>
#include <sstream>

#include <locale>
#include <codecvt>

#include "olcConsoleGameEngine.h"
#include "objecth.h"
#include "tdmath.h"
#include "debug.h"
#include "string.h"
#include "float.h"
#include "string.h"
#include "cyclone/interpreter.h"

using namespace std;

olcSprite*		defSprite = nullptr;

enum ObjSpritePattern
{
	OSP_NULL = 0,
	OSP_REPEATED = 1,
};

struct objmaterial
{
	std::string name;
	olcSprite* sprite = nullptr;

	objmaterial() {}

	objmaterial(std::string file)
	{
		//Name the material
		this->name = file;

		//Convert string to wstring
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(file);

		//Load the sprite
		sprite = new olcSprite(wide);

	}

	objmaterial(std::string file, std::string name)
	{
		this->name = name;

		//Convert string to wstring
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(file);

		//Load the sprite
		sprite = new olcSprite(wide);

	}

	objmaterial(std::wstring file, std::string name)
	{
		this->name = name;

		sprite = new olcSprite(file);

	}

};

objmaterial* defMaterial = nullptr;

enum objcollisionType
{
	OCT_NONE = 0,
	OCT_PLANE = 1,
	OCT_PRIMITIVE = 2,
};

struct object
{
	mesh object_mesh;
	mesh* object_collision_mesh = nullptr;										//Mainly a pointer so we can reference the main mesh back
	ObjSpritePattern spritePattern;
	mat4x4 position, rotation;
	float rx = 0, ry = 0, rz = 0;
	float lighting_range[2] = { 0.0f, 1.0f };
	bool visible = true;															//For map data, if the object is frozen

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

	vec3d getPosition()
	{
		vec3d pos;
		pos.x = position.m[3][0];
		pos.y = position.m[3][1];
		pos.z = position.m[3][2];
		return pos;
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

	bool setRotation(vec3d v)
	{
		return setRotation(v.x, v.y, v.z);
	}

};

olcSprite* getDefaultSprite()
{
	if (defSprite == nullptr)
	{
		defSprite = new olcSprite();
		defSprite->Load(L"resources\\default.png.spr");
	}

	return defSprite;
}

objmaterial* getDefaultMaterial()
{
	if (defMaterial == nullptr)
	{
		defMaterial = new objmaterial();
		defMaterial->sprite = getDefaultSprite();
		defMaterial->name = string("abortnite dance");
	}

	return defMaterial;
}

vector<object*> LoadFromObjectFile(string sFilename, bool bHasTexture = false)
{
	debugTabIndex++;
	vector<object*> objects;
	mesh tempmesh;
	vector<triangle3d> tris;
	wstring name;
	string mtlFilename = sFilename;

	//Remove the .obj if needed
	const string ext = ".obj";
	if (sFilename != ext &&
		sFilename.size() > ext.size() &&
		sFilename.substr(sFilename.size() - ext.size()) == ".obj")
	{
		// if so then strip them off
		mtlFilename = sFilename.substr(0, sFilename.size() - ext.size());
	}
	mtlFilename += ".mtl";

	//Check if the file isnt already open
	ifstream fmtl(mtlFilename);
	bool bFoundMtlAvaliable = false;
	if (fmtl.is_open())
		bFoundMtlAvaliable = true;

	ifstream f(sFilename);
	if (!f.is_open())
	{
		odprintf("ERROR: No such file found when loading OBJ: %s", sFilename.c_str());
		return vector<object*>();
	}

	// Local cache of verts
	vector<vec3d> verts;
	vector<vec2d> texs;
	vector<objmaterial* > materials;
	objmaterial* currMaterial = getDefaultMaterial();

	//Handle MTL File data
	string sline;
	stringstream ss;
	int i = 0;
	if (bFoundMtlAvaliable)
		while (!fmtl.eof())
		{
			//First get the line
			std::getline(fmtl, sline);

			//Check if the command is to create a new material
			string substrtochecl = string("newmtl ");
			std::size_t foundpos = sline.find(substrtochecl);
			if (foundpos != std::string::npos && foundpos == 0)
			{
				i++;
				string mname = sline.substr(substrtochecl.length(), sline.length() - substrtochecl.length());
				materials.push_back(new objmaterial());
				materials[materials.size() - 1]->name = mname;
				materials[materials.size() - 1]->sprite = getDefaultSprite();
			}

			//Check if the command is to load a material
			substrtochecl = string("map_Kd ");
			foundpos = sline.find(substrtochecl);
			if (foundpos != std::string::npos && foundpos == 0)
			{
				string mname = sline.substr(substrtochecl.length(), sline.length() - substrtochecl.length());
				
				std::stringstream test(mname);
				std::string segment;
				std::vector<std::string> seglist1;

				while (std::getline(test, segment, '\\'))
					seglist1.push_back(segment);

				mname = seglist1[seglist1.size() - 1];

				//materials.push_back(new objmaterial(mname, L"resources\\default.spr"));
				mname = "resources\\" + mname + ".spr";
				wstring wmname = to_wstring(mname);

				materials[materials.size() - 1]->sprite = new olcSprite(wmname);
				odprintf("Loading material sprite: %s", to_string(wmname).c_str());
				ifstream fmat(mname);
				if (!fmat.is_open())
				{
					odprintf("Warning: resource not found [%s]! Please make sure all of your resources are correct! Material [%i] has been defaulted!", mname.c_str(), i);
				}
			}


		}

	//Handle OBJ File data
	char line[512];
	i = 0;
	while (!f.eof())
	{
		f.getline(line, 512);

		strstream s;
		s << line;

		char junk;

		i++;

		if (line[0] == 'u' && line[3] == 'm' && line[5] == 'l')
		{
			for (int j = 0; j < materials.size(); j++)
			{
				if (materials[j]->name == string(line + 7))
				{
					currMaterial = materials[j];
				}
			}
		}

		if (line[0] == 'o')
		{
			if (objects.size() == 0 && name.length() == 0)
			{
				wstring stringline(&line[0], &line[512]);									//Get the current name of the object
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

				tempmesh = mesh{ tris, name };												//Set the mesh to 
				objects.push_back(new object(tempmesh));									//Add the mesh to the list
				tris.clear();																//Clear the tirangle list

				wstring stringline(&line[0], &line[128]);									//Get the current name of the object
				name = stringline.substr(2, stringline.length());
				odprintf("Loaded new object!");
			}

		}

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				vec2d v;
				s >> junk >> junk >> v.u >> v.v;
				// A little hack for the spyro texture
				// Another Note: Its a dam well good hack for anything else -_-
				//v.u = 1.0f - v.u;
				v.v = 1.0f - v.v;
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
				tris[tris.size() - 1].spritePtr = getDefaultMaterial()->sprite;

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

#ifndef _DEBUG

				if (currMaterial->sprite != getDefaultMaterial()->sprite)
				{
					tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
					tris[tris.size() - 1].spritePtr = currMaterial->sprite;
				}

#else
				tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
					texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
				tris[tris.size() - 1].spritePtr = currMaterial->sprite;

#endif // !_DEBUG

			}

		}
	}

	//We need to add the ending object to the list
	tempmesh = mesh{ tris, name };							//Set the mesh to 
	objects.push_back(new object(tempmesh));									//Add the mesh to the list


	debugTabIndex--;
	return objects;
}

object* ObjGeneratePlane(vec3d mid, size_t width, size_t height)
{
	object* objToReturn = new object();
	triangle3d t[2];

	/*=========================
	/ 
	/        1---2
	/        | / |
	/        3---4
	/
	/*=========================*/

	vec3d p1(mid.x - (width / 2), mid.y - (height / 2), 0);
	vec3d p2(mid.x - (width / 2), mid.y + (height / 2), 0);
	vec3d p3(mid.x + (width / 2), mid.y - (height / 2), 0);
	vec3d p4(mid.x + (width / 2), mid.y + (height / 2), 0);

	t[0].p[0] = p1;
	t[0].p[1] = p2;
	t[0].p[2] = p3;

	t[1].p[0] = p2;
	t[1].p[1] = p3;
	t[1].p[2] = p4;

	t[0].spritePtr = getDefaultSprite();
	t[1].spritePtr = getDefaultSprite();

	objToReturn->object_mesh.tris.push_back(t[0]);
	objToReturn->object_mesh.tris.push_back(t[1]);

	return objToReturn;
}

#endif