#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <condition_variable>

#include "olcConsoleGameEngine.h"
#include "object.h"
#include "sprite.h"
#include "tdmath.h"
#include "debug.h"

///OPENCL DATA///
#include "opencl.h"

using namespace std;

struct triangle_roastered
{
	olcSprite sprite;

	vec3d p[3];
	vec2d t[3]; // added a texture coord per vertex
	wchar_t sym;
	short col;
};

class Render
{
public:
	///MAIN RENDER DATA///
	CHAR_INFO* screenBuffer;
	short screenClearChar;
	int screenClearColor;
	olcSprite* screenSprite;
	olcSprite* screenSpriteBuffer[2];
	std::chrono::time_point<std::chrono::system_clock> tp1, tp2;
	std::thread mainThread;

	///MAIN CAMERA DATA///
	mat4x4 matProj;	// Matrix that converts from view space to screen space
	vec3d vCamera;	// Location of camera in world space
	vec3d vLookDir;	// Direction vector along the direction camera points
	float fYaw;		// FPS Camera rotation in XZ plane
	float fPitch;
	float fTheta;	// Spins World transform
	float* pDepthBuffer = nullptr;
	bool bRenderSpecificObject = false;
	int iRenderSpecificObject = 0;
	float fFPS = 0;
	long lElapsedFPS = 0;
	long lLastElapsedFPS = 0;
	float fTimeElapsedFPS = 0;
	float fElapsedTotal = 0.0f;
	bool bMustRender = false;
	bool bWFNFToggle = false;

	///MAIN WORLD DATA///
	vector<object*> objects;
	vector<olcSprite*> sprites;
	vector<vec3d*> lights;
	vector<triangle_roastered> tri_roastered;
	vector<triangle_roastered> tri_organized;

	///SCREEN DATA///
	int screenWidth, screenHeight;
	short ClearColor = FG_CYAN | BG_BLUE;
	
	float offset, fTotalTime;

	///OPENCL DATA///
#ifdef RENDER_GPU_ENABLED
	cl::Kernel clKernelRasterTriangle;
#endif

private:
	bool bRenderingThread = false;

public:
	int Initalize(int width, int height)
	{
		odprintf("Initalizing renderer instance [%d, %d]...!\n", width, height);
		debugTabIndex++;

		odprintf("Saving system varaibles...!\n");

		this->screenWidth = width;
		this->screenHeight = height;

		fTotalTime = 0;

		odprintf("Creating screen buffer [%d]...!\n", (screenWidth * screenHeight));

		pDepthBuffer = new float[(float)width * height];	//Setup the depth buffer

		// Projection Matrix
		matProj = Matrix_MakeProjection(90.0f, ((float)width/2) / ((float)height * 2), 0.1f, 1000.0f);

#ifdef RENDER_GPU_ENABLED
		// Handle the OpenGl
		clInitalize();

		clKernelRasterTriangle = clLoadKernel("RasterTriangle", "Main.cl");																												//Load the kernel
#endif

		odprintf("Creating screen sprite [%d]...!\n", (screenWidth * screenHeight));

		screenSpriteBuffer[0] = new olcSprite(screenWidth, screenHeight);
		screenSpriteBuffer[1] = new olcSprite(screenWidth, screenHeight);

		screenSprite = screenSpriteBuffer[0];

		debugTabIndex--;

		odprintf("Initalized renderer instance!");

		return 1;
	}

	int Update(float fElapsedTime)
	{

		return 1;
	}

public:

	bool RenderStart()
	{
		odprintf("Render[%p].RenderStart()...", this);

		debugTabIndex++;

		odprintf("Creating thread for constant rendering...!\n");

		mainThread = std::thread(&Render::RenderCPU, this);

		if (!mainThread.joinable())
		{
			odprintf("WARNING: mainThread is already running! Returning false...");
			return false;
		}

		mainThread.detach();																		//Detach the thread from the main thread

		while (!bRenderingThread)
		{
			odprintf("WAITING [%d]...", bRenderingThread);

			Sleep(1);

		}

		debugTabIndex--;

		return true;
	}

	bool RenderStop()
	{

	}

	bool RenderCPU()
	{
		tp1 = std::chrono::system_clock::now();														//Set the internal clock

		bRenderingThread = true;																	//Set the boolean to true

		odprintf("THREAD [Render.RenderCPU()] Called & Running...");

		while (bRenderingThread)																	//This will create a infinite while loop until we toggle the boolean
		{
			this->bWFNFToggle = !this->bWFNFToggle;

			try
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				
				float fElapsedTime = elapsedTime.count();
				float fSleepTime = fFPS - fElapsedTime;

				if (fSleepTime > 0) Sleep(fSleepTime);

				fElapsedTotal += fElapsedTime;

				if (screenSprite == nullptr)
				{
					odprintf("THREAD [Render.RenderCPU()] [%8.4f] ERROR: screenSprite == nullptr (possible exeptions to be thrown)!" );
					odprintf("THREAD [Render.RenderCPU()] [%8.4f] FIX:   recreated screenSprite into area.");

					screenSpriteBuffer[0] = new olcSprite(screenWidth, screenHeight);
					screenSpriteBuffer[1] = new olcSprite(screenWidth, screenHeight);

					screenSprite = screenSpriteBuffer[0];

				}

				//Swap the buffers and set the current buffer
				std::swap(screenSpriteBuffer[0], screenSpriteBuffer[1]);
				screenSprite = screenSpriteBuffer[1];

				mat4x4 matWorld;
				matWorld = Matrix_MakeIdentity();	// Form World Matrix

				// Create "Point At" Matrix for camera
				vec3d vUp = { 0,1,0 };
				vec3d vTarget = { 0,0,1 };
				mat4x4 matCameraRot;
				mat4x4 matRotX = Matrix_MakeRotationX(fPitch);
				mat4x4 matRotY = Matrix_MakeRotationY(fYaw);
				matCameraRot = Matrix_MultiplyMatrix(matRotX, matRotY);
				vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
				vTarget = Vector_Add(vCamera, vLookDir);
				mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

				// Make view matrix from camera
				mat4x4 matView = Matrix_QuickInverse(matCamera);

				// Store triagles for rastering later
				vector<triangle> vecTrianglesToRaster;

				float ftotalx = 0.0f, ftotaly = 0.0f, ftotalz = 0.0f;
				long ltotal = 0;

				// Draw Triangles
				for (int obsc = 0; obsc < objects.size(); obsc++)
					for (triangle tri : objects[obsc]->object_mesh.tris)
					{
						triangle triProjected, triTransformed, triViewed;

						mat4x4 meshTrans;

						meshTrans = Matrix_MultiplyMatrix(objects[obsc]->position, objects[obsc]->rotation);							// Transform by translation

						tri.p[0] = Matrix_MultiplyVector(meshTrans, tri.p[0]);
						tri.p[1] = Matrix_MultiplyVector(meshTrans, tri.p[1]);
						tri.p[2] = Matrix_MultiplyVector(meshTrans, tri.p[2]);

						// World Matrix Transform
						triTransformed.p[0] = Matrix_MultiplyVector(matWorld, tri.p[0]);
						triTransformed.p[1] = Matrix_MultiplyVector(matWorld, tri.p[1]);
						triTransformed.p[2] = Matrix_MultiplyVector(matWorld, tri.p[2]);
						triTransformed.t[0] = tri.t[0];
						triTransformed.t[1] = tri.t[1];
						triTransformed.t[2] = tri.t[2];

						// Calculate triangle Normal
						vec3d normal, line1, line2;

						// Get lines either side of triangle
						line1 = Vector_Sub(triTransformed.p[1], triTransformed.p[0]);
						line2 = Vector_Sub(triTransformed.p[2], triTransformed.p[0]);

						// Take cross product of lines to get normal to triangle surface
						normal = Vector_CrossProduct(line1, line2);

						// You normally need to normalise a normal!
						normal = Vector_Normalise(normal);

						// Get Ray from triangle to camera
						vec3d vCameraRay = Vector_Sub(triTransformed.p[0], vCamera);

						// If ray is aligned with normal, then triangle is visible
						if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
						{
							/*
							// Illumination
							vec3d light_direction = { 0.0f, 10.0f, 0.0f };
							light_direction = Vector_Normalise(light_direction);

							// How "aligned" are light direction and triangle surface normal?
							float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

							// Choose console colours as required (much easier with RGB)
							CHAR_INFO c = GetColour(dp);
							triTransformed.col = c.Attributes;
							triTransformed.sym = c.Char.UnicodeChar;
							*/

							float dp = 0.0f;																					//For storing the baisc dp

							for (vec3d* lp : lights)																			//Get the lighest light
							{
								vec3d light_direction = Vector_Normalise(*lp);

								// How "aligned" are light direction and triangle surface normal?
								if (dp < max(0.1f, Vector_DotProduct(light_direction, normal)))
									dp = max(0.1f, Vector_DotProduct(light_direction, normal));
							}

							float dprange = abs(objects[obsc]->lighting_range[0] - objects[obsc]->lighting_range[1]);			//Get the range	
							dprange *= dp;																						//Mulu by the largets light
							float dpmax = min(objects[obsc]->lighting_range[0], objects[obsc]->lighting_range[1]);				//Get the min of the two values
							dp = dpmax + dprange;																				//Set the new dp

							// Choose console colours as required (much easier with RGB)
							CHAR_INFO c = GetColour(dp);
							triTransformed.col = c.Attributes;
							triTransformed.sym = c.Char.UnicodeChar;

							// Convert World Space --> View Space
							triViewed.p[0] = Matrix_MultiplyVector(matView, triTransformed.p[0]);
							triViewed.p[1] = Matrix_MultiplyVector(matView, triTransformed.p[1]);
							triViewed.p[2] = Matrix_MultiplyVector(matView, triTransformed.p[2]);
							triViewed.sym = triTransformed.sym;
							triViewed.col = triTransformed.col;
							triViewed.t[0] = triTransformed.t[0];
							triViewed.t[1] = triTransformed.t[1];
							triViewed.t[2] = triTransformed.t[2];

							// Clip Viewed Triangle against near plane, this could form two additional
							// additional triangles. 
							int nClippedTriangles = 0;
							triangle clipped[2];
							nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

							// We may end up with multiple triangles form the clip, so project as
							// required
							for (int n = 0; n < nClippedTriangles; n++)
							{
								// Project triangles from 3D --> 2D
								triProjected.p[0] = Matrix_MultiplyVector(matProj, clipped[n].p[0]);
								triProjected.p[1] = Matrix_MultiplyVector(matProj, clipped[n].p[1]);
								triProjected.p[2] = Matrix_MultiplyVector(matProj, clipped[n].p[2]);
								triProjected.col = clipped[n].col;
								triProjected.sym = clipped[n].sym;
								triProjected.t[0] = clipped[n].t[0];
								triProjected.t[1] = clipped[n].t[1];
								triProjected.t[2] = clipped[n].t[2];


								triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
								triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
								triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

								triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
								triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
								triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

								triProjected.t[0].w = 1.0f / triProjected.p[0].w;
								triProjected.t[1].w = 1.0f / triProjected.p[1].w;
								triProjected.t[2].w = 1.0f / triProjected.p[2].w;


								// Scale into view, we moved the normalising into cartesian space
								// out of the matrix.vector function from the previous videos, so
								// do this manually
								triProjected.p[0] = Vector_Div(triProjected.p[0], triProjected.p[0].w);
								triProjected.p[1] = Vector_Div(triProjected.p[1], triProjected.p[1].w);
								triProjected.p[2] = Vector_Div(triProjected.p[2], triProjected.p[2].w);

								// X/Y are inverted so put them back
								triProjected.p[0].x *= -1.0f;
								triProjected.p[1].x *= -1.0f;
								triProjected.p[2].x *= -1.0f;
								triProjected.p[0].y *= -1.0f;
								triProjected.p[1].y *= -1.0f;
								triProjected.p[2].y *= -1.0f;

								// Offset verts into visible normalised space
								vec3d vOffsetView = { 1,1,0 };
								triProjected.p[0] = Vector_Add(triProjected.p[0], vOffsetView);
								triProjected.p[1] = Vector_Add(triProjected.p[1], vOffsetView);
								triProjected.p[2] = Vector_Add(triProjected.p[2], vOffsetView);
								triProjected.p[0].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[0].y *= 0.5f * (float)this->screenHeight;
								triProjected.p[1].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[1].y *= 0.5f * (float)this->screenHeight;
								triProjected.p[2].x *= 0.5f * (float)this->screenWidth;
								triProjected.p[2].y *= 0.5f * (float)this->screenHeight;

								triProjected.spritePtr = objects[obsc]->sprite;

								triProjected.triid = obsc;

								ftotalx += (triProjected.p[0].x + triProjected.p[1].x + triProjected.p[2].x) / 3;
								ftotaly += (triProjected.p[0].y + triProjected.p[1].y + triProjected.p[2].y) / 3;
								ftotalz += (triProjected.p[0].z + triProjected.p[1].z + triProjected.p[2].z) / 3;

								ltotal++;

								// Store triangle for sorting
								vecTrianglesToRaster.push_back(triProjected);
							}
						}

					}

				ftotalx /= ltotal;
				ftotaly /= ltotal;
				ftotalz /= ltotal;

				// Sort triangles from back to front
				sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
				{
					float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
					float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
					return z1 > z2;
				});
				
				// Clear Depth Buffer
				for (int i = 0; i < this->screenWidth * this->screenHeight; i++)
					pDepthBuffer[i] = 0.0f;

				// Clear Depth Buffer
				//memset(pDepthBuffer, 0, sizeof(float) * (this->screenWidth * this->screenHeight));

				// Loop through all transformed, viewed, projected, and sorted triangles
				for (auto& triToRaster : vecTrianglesToRaster)
				{
					// Clip triangles against all four screen edges, this could yield
					// a bunch of triangles, so create a queue that we traverse to 
					//  ensure we only test new triangles generated against planes
					triangle clipped[2];
					list<triangle> listTriangles;

					// Add initial triangle
					listTriangles.push_back(triToRaster);
					int nNewTriangles = 1;

					for (int p = 0; p < 4; p++)
					{
						int nTrisToAdd = 0;
						while (nNewTriangles > 0)
						{
							// Take triangle from front of queue
							triangle test = listTriangles.front();
							listTriangles.pop_front();
							nNewTriangles--;

							// Clip it against a plane. We only need to test each 
							// subsequent plane, against subsequent new triangles
							// as all triangles after a plane clip are guaranteed
							// to lie on the inside of the plane. I like how this
							// comment is almost completely and utterly justified
							switch (p)
							{
							case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
							case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)this->screenHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
							case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
							case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)this->screenWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
							}

							// Clipping may yield a variable number of triangles, so
							// add these new ones to the back of the queue for subsequent
							// clipping against next planes
							for (int w = 0; w < nTrisToAdd; w++)
							{
								clipped[w].spritePtr = test.spritePtr;
								clipped[w].triid = test.triid;
								listTriangles.push_back(clipped[w]);
							}
						}
						nNewTriangles = listTriangles.size();
					}

					// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
					for (auto& t : listTriangles)
					{
						if (bRenderSpecificObject)
							if (iRenderSpecificObject != t.triid) continue;

						if (t.spritePtr == nullptr || t.spritePtr->nHeight == 0 || t.spritePtr->nWidth == 0)
							SpriteFillTriangle(screenSpriteBuffer[0], t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
						else
							SpriteTexturedTriangle(screenSpriteBuffer[0], pDepthBuffer, screenWidth,
								t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
								t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
								t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,
								t.spritePtr);

						//SpriteDrawTriangle(screenSpriteBuffer[0], t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
						
					}
				}

				if (floorf(fTimeElapsedFPS) < floorf(fElapsedTotal))
				{
					fTimeElapsedFPS = fElapsedTotal;
					lLastElapsedFPS = lElapsedFPS;
					lElapsedFPS = 0;
					odprintf("%ld | %f %f %f | %f %f", lLastElapsedFPS, vCamera.x, vCamera.y, vCamera.z, fPitch, fYaw);
				}
				else
				{
					lElapsedFPS++;
				}

			}
			catch (exception e)
			{
				odprintf("ERROR: %c",  e.what());
			}
		
		}

		return true;
	}

	bool WaitForNextFrame()
	{
		odprintf("Render[%p].WaitForNextFrame()...", this);

		debugTabIndex++;

		bool currentB = bWFNFToggle;

		while (bWFNFToggle == currentB)
		{
			odprintf("%d | %d", bWFNFToggle, currentB);
			Sleep(1);
		}

		debugTabIndex--;

		return true;
	}

#ifdef RENDER_GPU_ENABLED
	bool RenderGPU(float fElapsedTime, olcConsoleGameEngine* e)
	{
		// Set up "World Tranmsform" though not updating theta 
		// makes this a bit redundant
		mat4x4 matRotZ, matRotX;
		fTheta = 0.5f;
		matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
		matRotX = Matrix_MakeRotationX(fTheta);

		mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

		mat4x4 matWorld;
		matWorld = Matrix_MakeIdentity();	// Form World Matrix
		matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); // Transform by rotation
		matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

		// Create "Point At" Matrix for camera
		vec3d vUp = { 0,1,0 };
		vec3d vTarget = { 0,0,1 };
		mat4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
		vLookDir = Matrix_MultiplyVector(matCameraRot, vTarget);
		vTarget = Vector_Add(vCamera, vLookDir);
		mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// Make view matrix from camera
		mat4x4 matView = Matrix_QuickInverse(matCamera);

		//My turn -Lrios4403

		/*---OPENCL TEST----------------------------------------------------------------------------------
		cl_triangle triCalc;
		cl::Buffer memBuf(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(triCalc));
		cl::Kernel kernel = clLoadKernel("TestTriangle", "Main.cl");
		kernel.setArg(0, memBuf);

		clCurrentCommandQue.enqueueTask(kernel);
		clCurrentCommandQue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(triCalc), &triCalc);

		cl_triangle triCalcs[1024];
		cl::Buffer memBuf(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_triangle) * 1024);
		cl::Kernel kernel = clLoadKernel("TestVectorTriangle", "Main.cl");
		kernel.setArg(0, memBuf);

		clCurrentCommandQue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024));
		clCurrentCommandQue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(cl_triangle) * 1024, &triCalcs);


		vector<cl_triangle> triCalcs(meshMountain.tris.size());
		cl::Buffer memBuf(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(cl_triangle) * triCalcs.size());
		cl::Kernel kernel = clLoadKernel("TestVectorTriangle", "Main.cl");
		kernel.setArg(0, memBuf);

		clCurrentCommandQue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(triCalcs.size()));
		clCurrentCommandQue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(cl_triangle) * triCalcs.size(), triCalcs.data());

		/*---OPENCL TEST----------------------------------------------------------------------------------
		*/

		vector<triangle> vecTrianglesToRaster;																																								//Set the triangle-to-raster vector
		int iScreenWidth = this->screenWidth, iScreenHeight = this->screenHeight;

		for (int i = 0; i < objects.size(); i++)
		{
			vecTrianglesToRaster.insert(vecTrianglesToRaster.end(), objects[i]->object_mesh.tris.begin(), objects[i]->object_mesh.tris.end());

		}

		int iVecRasterSize = vecTrianglesToRaster.size();

		vec3d light_direction = { 0.0f, 1.0f, -1.0f };																																						//Basic light direction

		cl::Buffer triBuffer(clCurrentContext, CL_MEM_READ_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(triangle) * iVecRasterSize, vecTrianglesToRaster.data());							//Set the buffer of the triangle
		cl::Buffer rotzBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(mat4x4), &matRotZ);																//Set the z rotation
		cl::Buffer rotxBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(mat4x4), &matRotX);																//Set the x rotation
		cl::Buffer vcamBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vec3d), &vCamera);																	//Set the vCamera 
		cl::Buffer swidthBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &iScreenWidth);															//Set the screen width 
		cl::Buffer sheightbuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &iScreenHeight);															//Set the screen height
		cl::Buffer projBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(mat4x4), &matProj);																//Set the screen projection
		cl::Buffer lightBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(vec3d), &light_direction);														//Set the light projection
		cl::Buffer matWorldBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(mat4x4), &matWorld);															//Set the the world matrix
		cl::Buffer matViewdBuffer(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(mat4x4), &matView);															//Set the the view matrix
		cl::Buffer triOutBufferSize(clCurrentContext, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &iVecRasterSize);														//Set the triangle output size
		cl::Buffer triOutBuffer(clCurrentContext, CL_MEM_READ_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(triangle) * vecTrianglesToRaster.size(), vecTrianglesToRaster.data());			//Set the triangle output

		clKernelRasterTriangle.setArg(0, triBuffer);																																						//Set the arguments
		clKernelRasterTriangle.setArg(1, rotzBuffer);
		clKernelRasterTriangle.setArg(2, rotxBuffer);
		clKernelRasterTriangle.setArg(3, vcamBuffer);
		clKernelRasterTriangle.setArg(4, swidthBuffer);
		clKernelRasterTriangle.setArg(5, sheightbuffer);
		clKernelRasterTriangle.setArg(6, projBuffer);
		clKernelRasterTriangle.setArg(7, lightBuffer);
		clKernelRasterTriangle.setArg(8, matWorldBuffer);
		clKernelRasterTriangle.setArg(9, matViewdBuffer);
		clKernelRasterTriangle.setArg(10, triOutBufferSize);
		clKernelRasterTriangle.setArg(11, triOutBuffer);

		clCurrentCommandQue.enqueueNDRangeKernel(clKernelRasterTriangle, cl::NullRange, cl::NDRange(vecTrianglesToRaster.size()));
		clCurrentCommandQue.enqueueReadBuffer(triOutBuffer, CL_TRUE, 0, sizeof(triangle) * vecTrianglesToRaster.size(), vecTrianglesToRaster.data());

		// Sort triangles from back to front
		int trisToErase = 0;
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				if (t1.calculated == false)
				{
					return false;
				}

				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});

		// Clear Screen
		e->Fill(0, 0, this->screenWidth, this->screenHeight, PIXEL_SOLID, FG_CYAN);

		// Clear Depth Buffer
		for (int i = 0; i < this->screenWidth * this->screenHeight; i++)
			pDepthBuffer[i] = 0.0f;


		// Loop through all transformed, viewed, projected, and sorted triangles
		for (auto& triToRaster : vecTrianglesToRaster)
		{
			if (triToRaster.calculated == false) continue;

			// Clip triangles against all four screen edges, this could yield
			// a bunch of triangles, so create a queue that we traverse to 
			//  ensure we only test new triangles generated against planes
			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each 
					// subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed
					// to lie on the inside of the plane. I like how this
					// comment is almost completely and utterly justified
					switch (p)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)this->screenHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)this->screenWidth - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto& t : listTriangles)
			{
				e->FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
				//DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_WHITE);
			}
		}

		return true;
	}
#else
	bool RenderGPU(float fElapsedTime, olcConsoleGameEngine* e) { return false; }
#endif

	void AddSprite(olcSprite* s)
	{
		sprites.push_back(s);
	}

	void AddObject(object* o)
	{
		objects.push_back(o);
	}

	object* FindObject(wstring name)
	{
		for (auto o : objects)
		{
			if (wcscmp(o->object_mesh.name.c_str(), name.c_str()))
				return o;
		}

		return nullptr;
	}

	void SetObjects(vector<object*> objects)
	{
		this->objects = objects;
	}

	void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		olcSprite* tex)
	{
		if (y2 < y1)
		{
			swap(y1, y2);
			swap(x1, x2);
			swap(u1, u2);
			swap(v1, v2);
			swap(w1, w2);
		}

		if (y3 < y1)
		{
			swap(y1, y3);
			swap(x1, x3);
			swap(u1, u3);
			swap(v1, v3);
			swap(w1, w3);
		}

		if (y3 < y2)
		{
			swap(y2, y3);
			swap(x2, x3);
			swap(u2, u3);
			swap(v2, v3);
			swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float tex_u, tex_v, tex_w;

		float dax_step = 0, dbx_step = 0,
			du1_step = 0, dv1_step = 0,
			du2_step = 0, dv2_step = 0,
			dw1_step = 0, dw2_step = 0;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
					if (tex_w > pDepthBuffer[i * screenWidth + j])
					{
						SpriteDraw(screenSprite, j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i * screenWidth + j] = tex_w;
					}
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					swap(ax, bx);
					swap(tex_su, tex_eu);
					swap(tex_sv, tex_ev);
					swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					if (tex_w > pDepthBuffer[i * screenWidth + j])
					{
						SpriteDraw(screenSprite, j, i, tex->SampleGlyph(tex_u / tex_w, tex_v / tex_w), tex->SampleColour(tex_u / tex_w, tex_v / tex_w));
						pDepthBuffer[i * screenWidth + j] = tex_w;
					}
					t += tstep;
				}
			}
		}
	}

};

#endif