#ifndef ZSPACE_ENGINE_TYPES_H
#define ZSPACE_ENGINE_TYPES_H  1

#include <string>


typedef unsigned char BYTE;
typedef unsigned int z_size_t;

struct cTexture
{
	std::wstring filename;
	long bmBitsPixel = 0;
	unsigned int bmHeight = 0;
	long bmPlanes = 0;
	long bmType = 0;
	unsigned int bmWidth = 0;
	unsigned int bmWidthBytes = 0;
	BYTE* bmpBuffer = 0;
	BYTE faceR = 0, faceG = 0, faceB = 0;
	bool hasAlpha = 0;
};


struct sprite
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float lx = 0.0f;
	float ly = 0.0f;
	float lz = 0.0f;
	float vx = 0.0f, vy = 0.0f, vz = 0.0f;
	bool p = false;
	bool visable = false;
	int liveTime = 0;
	bool distance_infinity = false;
	bool zbuffer = false;
	bool scale = false;
	float oscale = 1;
	int animation_start = 0;
	int animation_len = 0;
	float alpha = 0.0f;
	cTexture* boTexture = 0;
};

struct vertex_type
{
	float lx = 0.0f, ly = 0.0f, lz = 0.0f, lw = 0.0f;
	float wx = 0.0f, wy = 0.0f, wz = 0.0f;
	//float sx,sy;
	//float u,v;
};

struct uv_type
{
	float u = 0.0f, v = 0.0f;
};

struct polygon_type
{
	float nX = 0.0f;
	float nY = 0.0f;
	float nZ = 0.0f;
	float D = 0.0f;
	z_size_t SurfaceTexture = 0;

	vertex_type* vertex[3] = {};
	uv_type uv[3] = {};

	int visable = 0;
};

struct object_type
{
	z_size_t vertcount = 0;
	z_size_t polycount = 0;
	float scale = 0;
	float x = 0, y = 0, z = 0;
	float wx = 0, wy = 0, wz = 0;
	float an_X = 0.0f, an_Y = 0.0f, an_Z = 0.0f;
	float vax = 0, vay = 0, vaz = 0;
	float thrust = 0.0f, turnpitch = 0.0f;
	float vx = 0, vy = 0, vz = 0;
	float mass = 1;
	float bounds = 1;
	bool shoot = false;
	polygon_type* polygon = 0;
	vertex_type* vertex = 0;

	bool hidden = false;
};




struct world_type
{
	z_size_t objcount = 0;
	object_type* obj = 0;
};





struct clip_type
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
	float u = 0.0f, v = 0.0f;
	float u1 = 0.0f, v1 = 0.0f;
};

struct cliped_polygon_type
{
	unsigned int vertcount = 0;
	clip_type vertex[8] = {};
};


#endif /* ZSPACE_ENGINE_TYPES_H */