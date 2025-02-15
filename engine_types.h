#ifndef ZSPACE_ENGINE_TYPES_H
#define ZSPACE_ENGINE_TYPES_H  1

#include <cmath>
#include <string>
#include <SDL3/SDL.h>


typedef unsigned int z_size_t;
typedef unsigned int z_screen_t;	// used for x an y coordinates in to textures and the display
typedef unsigned char BYTE;

// Vector struct and all related vector math.
struct Vec3 {

	float x, y, z;

	inline Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}
	//float operator[](int i) const { return e[i]; }
	//float& operator[](int i) { return e[i]; }


	inline Vec3 operator-(const Vec3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	inline void operator-=(Vec3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	}


	inline Vec3 operator+(const Vec3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	inline void operator+=(Vec3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
	}

	inline Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }

	inline Vec3& operator*=(float t) {
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	inline Vec3 operator/(float s) const { s = 1 / s;  return { x * s, y * s, z * s }; }

	inline Vec3& operator/=(float t) {
		return *this *= 1 / t;
	}

	inline void offset(Vec3 delta)
	{
		*this += delta;
	}

	// Dot product
	inline float dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	inline float length() const {
		return std::sqrt(length_squared());
	}

	inline float length_squared() const {
		return x * x + y * y + z * z;
	}


	inline Vec3 unitary() const {
		float len = length();
		return { x * len, y * len, z * len };
	}

	// dot product, used to get cosine angle between two vectors.
	inline float cos_angle(const Vec3& other)
	{
		//return (x * other.x + y * other.y + z * other.z) / (sqrt(x * x + y * y + z * z) * sqrt(other.x * other.x + other.y * other.y + other.z * other.z));
		return dot(other) / (length() * other.length());
	}

	inline Vec3 cross_product(Vec3& B)
	{
		return { +(y * B.z - z * B.y),
				-(x * B.z - z * B.x),
				+(x * B.y - y * B.x) };
	}

	void inline roll360()
	{
		if (x > 359) x = x - 360;
		if (x < 0) x = x + 360;
		if (y > 359) y = y - 360;
		if (y < 0) y = y + 360;
		if (z > 359) z = z - 360;
		if (z < 0) z = z + 360;
	}


};

template <typename T>
struct Colour {

	union {
		struct {
			T a, b, g, r;
		};

		T val;
	};

	inline Colour<T> operator*(float s) const { return { r * s, g * s, b * s }; }

	inline Colour<T>& operator*=(float t) {
		r *= t;
		g *= t;
		b *= t;
		return *this;
	}

	inline Colour<T> operator/(float s) const { s = 1 / s;  return *this * s; }
	
	inline Colour<T>& operator/=(float t) {
		return *this *= 1 / t;
	}

	inline void fromFloatC(Colour<float> fcolour)
	{
		r = static_cast <BYTE>(fcolour.r);
		g = static_cast <BYTE>(fcolour.g);
		b = static_cast <BYTE>(fcolour.b);
		//a = static_cast <BYTE>(fcolour.a);
	}

	inline void limit_rgba() {
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		//if (a < 0) a = 0;
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		//if (a > 255) a = 255;

	}
	
};

struct vertex_type
{
	Vec3 l = {0, 0, 0};
	Vec3 w = { 0,0,0 };
};

struct uv_type
{
	float u = 0.0f, v = 0.0f;
};

struct polygon_type
{
	Vec3 n = { 0,0,0 };
	float D = 0.0f;
	z_size_t SurfaceTexture = 0;

	vertex_type* vertex[3] = {};
	uv_type uv[3] = {};

	int visable = 0;

	inline void compute_normal()
	{
		float dAx = (vertex[1]->w.x - vertex[0]->w.x);
		float dAy = (vertex[1]->w.y - vertex[0]->w.y);
		float dAz = (vertex[1]->w.z - vertex[0]->w.z);

		float dBx = (vertex[2]->w.x - vertex[0]->w.x);
		float dBy = (vertex[2]->w.y - vertex[0]->w.y);
		float dBz = (vertex[2]->w.z - vertex[0]->w.z);

		n.x = (dAy * dBz - dAz * dBy);
		n.y = (dAz * dBx - dAx * dBz);
		n.z = (dAx * dBy - dAy * dBx);
		
		D = n.cos_angle(vertex[0]->w);
		//n = g_objects_shadowers[b].dA.cross_product(g_objects_shadowers[b].dB);
	}

};



struct cTexture
{
	std::wstring filename;
	SDL_Surface* surface = NULL;
	Colour<BYTE>* pixels = NULL;
	unsigned char bmBitsPixel = 0;
	unsigned char bmBytesPixel = 0;
	uint16_t bmHeight = 0;
	long bmType = 0;
	uint16_t bmWidth = 0;
	uint16_t bmWidthBytes = 0;
	unsigned char faceR = 0, faceG = 0, faceB = 0;
	bool hasAlpha = 0;
};



struct sprite
{
	Vec3 position = { 0,0,0 };
	Vec3 l = { 0,0,0 };
	Vec3 v = { 0,0,0 };
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



struct object_type
{
	z_size_t vertcount = 0;
	z_size_t polycount = 0;
	float scale = 0;
	Vec3 position = { 0,0,0 };	// Object 3d world position.
	Vec3 wposition = { 0,0,0 }; // temp position?
	Vec3 angle = { 0,0,0 };	// Object orientation around position.
	Vec3 van = { 0,0,0 };	
	Vec3 va = { 0,0,0 }; // Angular velocity in degrees per frame..
	float thrust = 0.0f;
	float turnpitch = 0.0f;
	Vec3 v = { 0,0,0 }; // ??
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
	Vec3 position = { 0,0,0 };
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