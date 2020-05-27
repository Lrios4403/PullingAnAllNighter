#ifndef OBJECTH_H
#define OBJECTH_H

#include <iostream>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <string>
#include <codecvt>

#include "olcConsoleGameEngine.h"
#include "debug.h"

using namespace std;

inline bool sameSign(float a, float b, float c) {
	return (a > 0.f && b > 0.f && c > 0.f) ||
		(a < 0.f && b < 0.f && c < 0.f);
}

inline bool sameSign(float a, float b) {
	return (a > 0.f && b > 0.f) ||
		(a < 0.f && b < 0.f);
}

inline bool overlaps(float min1, float max1, float min2, float max2)
{
	// misses:
	// miss 1: max1 < min2
	// 11111
	//        22222
	// miss 2: max2 < min1
	// 2222
	//       1111
	//return  isBetweenOrdered( min1, min2, max2 ) || // case 1 or 3
	//        isBetweenOrdered( min2, min1, max1 ) ;  // case 2 or 4
	//
	// this is the opposite of the `misses` cases.  condn for !miss: !( max1 < min2 || max2 < min1 ) ;
	// It doesn't make that much sense if you look at it without considering that it is just
	// the logical inversion of the miss cases.  !miss=overlap.
	return max1 >= min2 && max2 >= min1;
}

// Created a 2D structure to hold texture coordinates
struct vec2d
{
	union
	{
		float x;
		float u;
	};

	union
	{
		float y;
		float v;
	};

	float w = 1;

	vec2d(float u = 0, float v = 0, float w = 1)
	{
		this->u = u;
		this->v = v;
		this->w = w;
	}

	vec2d operator+(const vec2d& a) const
	{
		return vec2d{ a.x + x, a.y + y, w };
	}

	vec2d operator-(const vec2d& a) const
	{
		return vec2d{ a.x - x, a.y - y, w };
	}

	vec2d operator*(const vec2d& a) const
	{
		return vec2d{ a.x * x, a.y * y, w };
	}

	vec2d operator/(const vec2d& a) const
	{
		return vec2d{ a.x / x, a.y / y, w };
	}

	vec2d operator+(const float& a) const
	{
		return vec2d{ a + x, a + y,  w };
	}

	vec2d operator-(const float& a) const
	{
		return vec2d{ a - x, a - y, w };
	}

	vec2d operator*(const float& a) const
	{
		return vec2d{ a * x, a * y, w };
	}

	vec2d operator/(const float& a) const
	{
		return vec2d{ a / x, a / y, w };
	}

	void operator +=(const vec2d& v)
	{
		this->u += v.u;
		this->v += v.v;
	}

	void operator -=(const vec2d& v)
	{
		this->u -= v.u;
		this->v -= v.v;
	}

	void operator /=(const vec2d& v)
	{
		this->u /= v.u;
		this->v /= v.v;
	}

};

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1; // Need a 4th term to perform sensible matrix vector multiplication

	vec3d(float x = 0, float y = 0, float z = 0, float w = 1)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	bool operator==(vec3d a) const
	{
		if (a.x == x && a.y == y && a.z == z && a.w == w)
			return true;
		else
			return false;
	}

	vec3d operator+(const vec3d& a) const
	{
		return vec3d{ a.x + x, a.y + y, a.z + z, w };
	}

	vec3d operator-(const vec3d& a) const
	{
		return vec3d{ a.x - x, a.y - y, a.z - z, w };
	}

	vec3d operator*(const vec3d& a) const
	{
		return vec3d{ a.x * x, a.y * y, a.z * z, w };
	}

	vec3d operator/(const vec3d& a) const
	{
		return vec3d{ a.x / x, a.y / y, a.z / z, w };
	}

	vec3d operator+(const float& a) const
	{
		return vec3d{ a + x, a + y, a + z, w };
	}

	vec3d operator-(const float& a) const
	{
		return vec3d{ a - x, a - y, a - z, w };
	}

	vec3d operator*(const float& a) const
	{
		return vec3d{ a * x, a * y, a * z, w };
	}

	vec3d operator/(const float& a) const
	{
		return vec3d{ a / x, a / y, a / z, w };
	}

	void operator /=(const float& a)
	{
		x /= a;
		y /= a;
		z /= a;
	}

	inline bool all(float val) const { return x == val && y == val && z == val; }
	inline bool allEqual() const { return x == y && x == z; }
	inline bool nonzero() const { return x || y || z; }
	// the SQUARE must be nonzero. because for REALLY small x,
	// x*x can be 0.
	inline bool nonzero2() const { return x * x || y * y || z * z; }
	// Note: negative (-0.0) works EXACTLY like +0.0
	inline bool allzero() const { return !x && !y && !z; }
	inline float len() const {
		return sqrtf(x * x + y * y + z * z);
	}
	inline vec3d cross(const vec3d& o) const {
		return vec3d(y * o.z - o.y * z, z * o.x - x * o.z, x * o.y - o.x * y);
	}
	inline float dot(const vec3d& o) const {
		return x * o.x + y * o.y + z * o.z;
	}
	inline vec3d& normalize() {
		float length = len();
		vec3d vtr = (*this);

		// I added this debug check man, never take it out.
		if (!length) {
			odprintf("vec3d::normalize() attempt to divide by 0");
			return *this;
		}

		vtr /= length;
		return vtr;
	}
	inline vec3d fabsCopy() const {
		return vec3d(fabsf(x), fabsf(y), fabsf(z));
	}
	inline int fabsMaxIndex() const {
		vec3d c = fabsCopy();
		if (c.x >= c.y && c.x >= c.z)  return 0;
		else if (c.y >= c.x && c.y >= c.z) return 1;
		else return 2;
	}

};

struct segment3d
{
	vec3d p[2];
};

struct segment2d
{
	vec3d p[2];
};

struct plane
{
	vec3d normal;
	float d;

	plane()
	{

	}

	plane(const vec3d& a, const vec3d& b, const vec3d& c)
	{
		recomputePlane(a, b, c);
	}

	inline float distanceToPoint(const vec3d& p) const
	{
		return normal.dot(p) + d;
	}

	void recomputePlane(const vec3d& A, const vec3d& B, const vec3d& C)
	{
		// the normal is just the cross of 
		// 2 vectors in the plane, normalized
		vec3d BA = A - B;
		vec3d BC = C - B;

		normal = BC.cross(BA);

		if (normal.allzero())
		{
			//error( "Triangle has 3 vertices that are collinear, (ie two vertices may be the same) the normal is 0" ) ;
			d = 0;// zero normal means d=0 as well
			return;
			//__debugbreak() ;
		}

		// now normalize this.
		normal.normalize();

		// http://en.wikipedia.org/wiki/Line-plane_intersection
		// use the normal to compute the d
		d = -normal.dot(A);
	}

};

struct triangle3d
{
	vec3d p[3];
	vec2d t[3]; // added a texture coord per vertex
	vec3d* a = &p[0], * b = &p[1], * c = &p[2];
	int triid = 0;
	wchar_t sym;
	short col;
	bool calculated = false;
	olcSprite* spritePtr;
	plane plane;

	// Ok, I need to get the odd one out. which index one has the different sign?
	// Here i'm assuming that you already checked `signDiffers` so at one has
	// a different sign.
	// 0 means a was different, 1 means b was, and 2 means c was
	inline int whichDifferent(float a, float b, float c) {
		if (sameSign(b, c))  return 0;
		else if (sameSign(a, c))  return 1;
		else //if( sameSign(a, b) )  
			return 2;
	}
	inline void recomputePlane()
	{
		plane.recomputePlane(p[0], p[1], p[2]);
	}

	bool CollisionWith(triangle3d& tri)
	{
		recomputePlane();
		tri.recomputePlane();

		// Since I need the distances, i compute and keep them here
		float dist1[3] = { tri.plane.distanceToPoint(*a), tri.plane.distanceToPoint(*b), tri.plane.distanceToPoint(*c) };

		// if all 3 pts of 1 tri are on the same side of the other tri, then
		// no intn can happen
		if (sameSign(dist1[0], dist1[1], dist1[2]))  return 0;

		float dist2[3] = { plane.distanceToPoint(*tri.a), plane.distanceToPoint(*tri.b), plane.distanceToPoint(*tri.c) };
		if (sameSign(dist2[0], dist2[1], dist2[2]))  return 0;

		// the paper says you don't need O.
		vec3d D = plane.normal.cross(tri.plane.normal).normalize();
		if (D.allzero()) {
			// tris parallel planes.  very rare for 2 tris to actually
			// be coplanar AND intersect, so i consider this a miss.
			//info( "parallel tris" ) ;
			return 0;
		}

		float p1[3] = { D.dot(*a), D.dot(*b), D.dot(*c) };

		int diffPt1 = whichDifferent(dist1[0], dist1[1], dist1[2]);
		//int o1 = OTHERAXIS1(diffPt1), o2 = OTHERAXIS2(diffPt1);
		int o1 = ((diffPt1 + 1) % 3);
		int o2 = ((diffPt1 + 2) % 3);

		// The 2 t's we want are the ones that cross over the tri plane.
		//    1 +
		//   / \  
		//  /   \  
		// 0-----2  
		// -     -
		// In this case, diffPt1=1, and o1=2, o2=0.
		float t1[2] = {
		  p1[diffPt1] + (p1[o1] - p1[diffPt1]) * dist1[diffPt1] / (dist1[diffPt1] - dist1[o1]), // diffPt1->o1
		  p1[diffPt1] + (p1[o2] - p1[diffPt1]) * dist1[diffPt1] / (dist1[diffPt1] - dist1[o2])  // diffPt1->o2
		};

		// Put them in order
		if (t1[0] > t1[1])  swap(t1[0], t1[1]);
		diffPt1 = whichDifferent(dist2[0], dist2[1], dist2[2]);
		o1 = ((diffPt1 + 1) % 3);
		o2 = ((diffPt1 + 2) % 3);
		float p2[3] = { D.dot(*tri.a), D.dot(*tri.b), D.dot(*tri.c) };
		//		float p2[3] = { tri.a.elts[axis], tri.b.elts[axis], tri.c.elts[axis] };
		float t2[2] = {
		  p2[diffPt1] + (p2[o1] - p2[diffPt1]) * dist2[diffPt1] / (dist2[diffPt1] - dist2[o1]), // diffPt1->o1
		  p2[diffPt1] + (p2[o2] - p2[diffPt1]) * dist2[diffPt1] / (dist2[diffPt1] - dist2[o2])  // diffPt1->o2
		};
		if (t2[0] > t2[1])  swap(t2[0], t2[1]);

		return overlaps(t1[0], t1[1], t2[0], t2[1]);

	}
};

struct triangle2d
{
	vec2d p[3];
	vec2d t[3]; // added a texture coord per vertex
	vec2d* a = &p[0], * b = &p[1], * c = &p[2];

	triangle2d()
	{

	}
};

struct rectangle
{
	vec2d p[4];
	vec2d offset;

	rectangle()
	{

	}

	rectangle(float x, float y, float width, float height)
	{
		p[0].u = x;
		p[0].v = y;

		p[1].u = x + width;
		p[1].v = y;

		p[2].u = x;
		p[2].v = y + height;

		p[3].u = x + width;
		p[3].v = y + height;

		Reraster();
	}

	void Reraster()
	{
		float fxmin = INFINITY;
		float fymin = INFINITY;
		float fxmax = -INFINITY;
		float fymax = -INFINITY;

		if (p[0].u < fxmin) fxmin = p[0].u;
		if (p[0].u > fxmax) fxmax = p[0].u;
		if (p[0].v < fymin) fymin = p[0].v;
		if (p[0].v > fymax) fymax = p[0].v;

		if (p[1].u < fxmin) fxmin = p[1].u;
		if (p[1].u > fxmax) fxmax = p[1].u;
		if (p[1].v < fymin) fymin = p[1].v;
		if (p[1].v > fymax) fymax = p[1].v;

		if (p[2].u < fxmin) fxmin = p[2].u;
		if (p[2].u > fxmax) fxmax = p[2].u;
		if (p[2].v < fymin) fymin = p[2].v;
		if (p[2].v > fymax) fymax = p[2].v;

		if (p[3].u < fxmin) fxmin = p[3].u;
		if (p[3].u > fxmax) fxmax = p[3].u;
		if (p[3].v < fymin) fymin = p[3].v;
		if (p[3].v > fymax) fymax = p[3].v;

		p[0].u = fxmin;
		p[0].v = fymin;

		p[1].u = fxmax;
		p[1].v = fymin;

		p[2].u = fxmin;
		p[2].v = fymax;

		p[3].u = fxmax;
		p[3].v = fymax;
	}


	bool bFastCollidsWith(rectangle& r)
	{
		float RectALeft = p[0].u + offset.u;
		float RectARight = p[3].u + offset.u;
		float RectATop = p[0].v + offset.v;
		float RectABottom = p[3].v + offset.v;

		float RectBLeft = r.p[0].u + r.offset.u;
		float RectBRight = r.p[3].u + r.offset.u;
		float RectBTop = r.p[0].v + r.offset.v;
		float RectBBottom = r.p[3].v + r.offset.v;

		bool b1 = RectALeft < RectBRight;
		bool b2 = RectARight > RectBLeft;
		bool b3 = RectATop < RectBBottom;
		bool b4 = RectABottom > RectBTop;

		if (b1 && b2 &&
			b3 && b4)
		{

			return true;
		}

		return false;
	}

};

struct mesh
{
	vector<triangle3d> tris;
	wstring name;

	bool LoadFromObjectFile(string sFilename, bool bHasTexture = false)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<vec3d> verts;
		vector<vec2d> texs;

		int i = 0;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			i++;

			if (line[0] == 'o')
			{
				name = wstring(&line[0], &line[128]);
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

		return true;

	}

	rectangle GetBounderies()
	{
		rectangle r;
		float fxmin = INFINITY, fxmax = -INFINITY;
		float fymin = INFINITY, fymax = -INFINITY;

		for (auto t : tris)
		{
			for (int j = 0; j < 3; j++)
			{
				if (t.p[j].x > fxmax) fxmax = t.p[j].x;
				if (t.p[j].y > fymax) fymax = t.p[j].y;

				if (t.p[j].x < fxmin) fxmin = t.p[j].x;
				if (t.p[j].y < fymin) fymin = t.p[j].y;
			}
		}

		r.p[0].u = fxmin;
		r.p[0].v = fymin;

		r.p[1].u = fxmax;
		r.p[1].v = fymin;

		r.p[2].u = fxmin;
		r.p[2].v = fymax;

		r.p[3].u = fxmax;
		r.p[3].v = fymax;

		return r;
	}

	bool Resize(float f)
	{
		for (int i = 0; i < tris.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				tris[i].p[j].x *= f;
				tris[i].p[j].y *= f;
				tris[i].p[j].z *= f;
			}
		}

		return true;
	}

};

struct mat4x4
{
	float m[4][4] = { 0 };
};

#endif