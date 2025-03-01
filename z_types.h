#pragma once

#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include "z_math.h"

#include "json.hpp"


typedef unsigned int z_size_t;
typedef unsigned int z_screen_t;	// used for x an y coordinates in to textures and the display
typedef unsigned char BYTE;


struct UV
{
	float u = 0.0f, v = 0.0f;

	void fromJSON(json::JSON& j)
	{
		if (j.at(0).JSONType() == json::JSON::Class::Integral)
			u = static_cast<float>(j.at(0).ToInt());
		else
			u = static_cast<float>(j.at(0).ToFloat());

		if (j.at(1).JSONType() == json::JSON::Class::Integral)
			v = static_cast<float>(j.at(1).ToInt());
		else
			v = static_cast<float>(j.at(1).ToFloat());
	}

};


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

	inline float length() const {
		return std::sqrt(length_squared());
	}

	inline float length_squared() const {
		return x * x + y * y + z * z;
	}


	inline Vec3 unitary() const {
		float len = 1/length();
		return { x * len, y * len, z * len };
	}

	// Dot product
	inline float dot(const Vec3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// dot product, used to get cosine angle between two vectors.
	inline float cos_angle(const Vec3& other)
	{
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
		x = fmod(x, 360.0f);
		y = fmod(y, 360.0f);
		z = fmod(z, 360.0f);
	}
	/*
	inline void fromJSON(JSON& j)
	{
		x = static_cast<float>(j.at(0).ToFloat());
		y = static_cast<float>(j.at(1).ToFloat());
		z = static_cast<float>(j.at(2).ToFloat());
	}
	*/
	void fromJSON(json::JSON& j)
	{
		if (j.at(0).JSONType() == json::JSON::Class::Integral)
			x = static_cast<float>(j.at(0).ToInt());
		else
			x = static_cast<float>(j.at(0).ToFloat());

		if (j.at(1).JSONType() == json::JSON::Class::Integral)
			y = static_cast<float>(j.at(1).ToInt());
		else
			y = static_cast<float>(j.at(1).ToFloat());

		if (j.at(2).JSONType() == json::JSON::Class::Integral)
			z = static_cast<float>(j.at(2).ToInt());
		else
			z = static_cast<float>(j.at(2).ToFloat());
	}

};


struct Matrix44 {
	float m[4][4];

	Matrix44()
	{
		ident();
	}

	inline void ident()
	{
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	}

	inline void multiply(float m2[4][4])
	{

		float temp[4][4] = { 0 };

		for (unsigned char i = 0; i < 4; i++)
			for (unsigned char j = 0; j < 4; j++)
				for (unsigned char k = 0; k < 4; k++)
					temp[i][j] += (m[i][k] * m2[k][j]);


		for (unsigned char i = 0; i < 4; i++)
		{
			m[i][0] = temp[i][0];
			m[i][1] = temp[i][1];
			m[i][2] = temp[i][2];
			m[i][3] = temp[i][3];
		}
	}

	inline void rotate(Vec3& Ang)
	{
		float rmat[4][4];

		// X
		rmat[0][0] = 1;		rmat[0][1] = 0;				rmat[0][2] = 0;				rmat[0][3] = 0;
		rmat[1][0] = 0;		rmat[1][1] = MyCos(Ang.x);	rmat[1][2] = MySin(Ang.x);	rmat[1][3] = 0;
		rmat[2][0] = 0;		rmat[2][1] = -MySin(Ang.x);	rmat[2][2] = MyCos(Ang.x);	rmat[2][3] = 0;
		rmat[3][0] = 0;		rmat[3][1] = 0;				rmat[3][2] = 0;				rmat[3][3] = 1;
		multiply(rmat);
		// Z
		rmat[0][0] = MyCos(Ang.z);	rmat[0][1] = MySin(Ang.z);	rmat[0][2] = 0;		rmat[0][3] = 0;
		rmat[1][0] = -MySin(Ang.z);	rmat[1][1] = MyCos(Ang.z);	rmat[1][2] = 0;		rmat[1][3] = 0;
		rmat[2][0] = 0;				rmat[2][1] = 0;				rmat[2][2] = 1;		rmat[2][3] = 0;
		rmat[3][0] = 0;				rmat[3][1] = 0;				rmat[3][2] = 0;		rmat[3][3] = 1;
		multiply(rmat);

		// Y
		rmat[0][0] = MyCos(Ang.y);	rmat[0][1] = 0;		rmat[0][2] = -MySin(Ang.y);	rmat[0][3] = 0;
		rmat[1][0] = 0;				rmat[1][1] = 1;		rmat[1][2] = 0;				rmat[1][3] = 0;
		rmat[2][0] = MySin(Ang.y);	rmat[2][1] = 0;		rmat[2][2] = MyCos(Ang.y);	rmat[2][3] = 0;
		rmat[3][0] = 0;				rmat[3][1] = 0;		rmat[3][2] = 0;				rmat[3][3] = 1;
		multiply(rmat);



	}

	inline void translate(Vec3& p)
	{
		float tmat[4][4];

		tmat[0][0] = 1; tmat[0][1] = 0; tmat[0][2] = 0; tmat[0][3] = 0;
		tmat[1][0] = 0; tmat[1][1] = 1; tmat[1][2] = 0; tmat[1][3] = 0;
		tmat[2][0] = 0; tmat[2][1] = 0; tmat[2][2] = 1; tmat[2][3] = 0;
		tmat[3][0] = p.x; tmat[3][1] = p.y; tmat[3][2] = p.z; tmat[3][3] = 1;
		multiply(tmat);
	}

	inline void transform(Vec3& p)
	{
		float outX = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + m[3][0];
		float outY = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + m[3][1];
		float outZ = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + m[3][2];
		p.x = outX; p.y = outY; p.z = outZ;
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
		a *= t;
		return *this;
	}

	inline Colour<T>& operator*(float t) {
		r *= t;
		g *= t;
		b *= t;
		a *= t;
		return *this;
	}

	inline Colour<T> operator/(float s) const { s = 1 / s;  return *this * s; }

	inline Colour<T>& operator/=(float t) {
		return *this *= 1 / t;
	}

	inline void operator+=(Colour<T>& c) {
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
	}


	inline float luminance()
	{
		return 0.299 * r + 0.587 * g + 0.114 * b;
		//return std::max(r, std::max(b, g));
	}

	inline void fromFloatC(Colour<float> fcolour)
	{
		r = static_cast <BYTE>(fcolour.r * 255);
		g = static_cast <BYTE>(fcolour.g * 255);
		b = static_cast <BYTE>(fcolour.b * 255);
		a = static_cast <BYTE>(fcolour.a * 255);
	}

	inline Vec3 toNormal() const {
		return {
			(static_cast<float>(r) - 255) / 127,
			(static_cast<float>(g) - 255) / 127,
			(static_cast<float>(b) - 255) / 127
		};
	}

	inline bool is_saturated() {
		if constexpr (std::is_floating_point_v<T>) {
			return (r == 1 && g == 0 && b == 1);
		}
		else {
			return (r == 255 && g == 0 && b == 255);
		}
	}

	inline void limit_rgba() {
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		if (a < 0) a = 0;
		if constexpr (std::is_floating_point_v<T>) {
			if (r > 1 || g > 1 || b > 1) {
				r = 1;
				g = 0;
				b = 1;
			}
			//if (g > 1) g = 0;
			//if (b > 1) b = 1;
			if (a > 1) a = 1;
		}
		else {
			if (r > 255 || g > 255 || b > 255) {
				r = 255;
				g = 0;
				b = 255;
			}
			if (a > 255) a = 255;
		}
	}


	void fromJSON(json::JSON& j)
	{
		if (j.at(0).JSONType() == json::JSON::Class::Integral)
			r = static_cast<float>(j.at(0).ToInt());
		else
			r = static_cast<float>(j.at(0).ToFloat());

		if (r == -1) {
			if(typeid(T) == typeid(BYTE))
			{
				*this = { 255, 178, 178, 178 };
			}
			else {
				*this = { 1, 0.7f , 0.7f , 0.7f };	// This is the default defuse colour.
			}
			
			return;
		}

		if (j.at(1).JSONType() == json::JSON::Class::Integral)
			g = static_cast<float>(j.at(1).ToInt());
		else
			g = static_cast<float>(j.at(1).ToFloat());

		if (j.at(2).JSONType() == json::JSON::Class::Integral)
			b = static_cast<float>(j.at(2).ToInt());
		else
			b = static_cast<float>(j.at(2).ToFloat());

		if (j.at(3).JSONType() == json::JSON::Class::Integral)
			a = static_cast<float>(j.at(3).ToInt());
		else
			a = static_cast<float>(j.at(3).ToFloat());
	}
};



// Our object structure defined either a plane or sphere. 
struct ZRay_Object
{
	z_size_t idx;
	int pType;
	Vec3 s;
	Vec3 dA;
	float dA_len;
	Vec3 dB;
	float dB_len;
	Vec3 dAu;
	Vec3 dBu;
	Colour<float> colour;
	Vec3 n;
	Vec3 nu;
	long SurfaceTexture;
	float SurfaceMultW;
	float SurfaceMultH;
	float radius_squared;
	bool casts_shadows = true;
	z_size_t linked_light = 0;

	inline void pre_compute() {
		dA_len = dA.length();
		dAu = dA.unitary();
		dB_len = dB.length();
		dBu = dB.unitary();
		n = dA.cross_product(dB);
		nu = n.unitary();
		
	}

	inline float InterPlane(Vec3 &o, Vec3 &r, Vec3 &out_intersect, UV &uv)
	{
		float D = r.dot(n);

		if (D < 0) return 0;
		Vec3 sr = s-o;
		Vec3 tcross = sr.cross_product(r);  // scalar triple product
		float D2 = dB.dot(tcross);			
		if ((D2 < 0) || (D2 > D)) return 0;
		float D3 = -dA.dot(tcross);

		if (D3 >= 0 && D3 <= D) {
			uv.u = D2 / D;
			uv.v = D3 / D;
			float L = sr.dot(n) / D;
			out_intersect.x = o.x + r.x * L;
			out_intersect.y = o.y + r.y * L;
			out_intersect.z = o.z + r.z * L;
			return L;
		}

		//out_intersect = { 0,0,0 };
		return 0;
	}


	inline float InterSphere(const Vec3& origin, const Vec3& dir, const float& an_Y, Vec3& out_intersect, UV& uv)
	{
		static bool first_hit = false;
		Vec3 intercept;

		Vec3 L = s - origin;

		Vec3 ndir = dir.unitary();

		// Assuming 'dir' is normalized, a can be 1.
		float a = ndir.dot(ndir);
		float b = 2.0 * ndir.dot(L);
		float c = L.dot(L) - radius_squared;

		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0) // No real roots: the ray misses the sphere.
			return 0;

		float sqrtDiscriminant = std::sqrt(discriminant);

		// Two possible solutions for t.
		float t0 = (b - sqrtDiscriminant) / (2 * a);
		float t1 = (b + sqrtDiscriminant) / (2 * a);

		// Ensure t0 is the smaller value.
		if (t0 > t1)
			std::swap(t0, t1);

		if (t0 < 0) {
			if (t1 < 0) // Both intersections are behind the ray.
				return 0;
			t0 = t1;
		}

		// Compute the intersection point: origin + t * direction.
		intercept = origin + ndir * t0;

		out_intersect = intercept;

		intercept = (intercept - s).unitary();

		//intercept = unit_vector(intercept);
		float qx = intercept.x, qy = intercept.y, qz = intercept.z;
		worldToLocal(intercept.x, intercept.y, intercept.z, 0.5*an_Y * 0.0174533, qx, qy, qz);



		uv.u = (std::atan2(qz, qx) + Pi) / (2.0f * Pi);
		uv.v = std::acos(qy) / Pi;

		return t0;

	}

};

struct Camera
{
	ZRay_Object screen;// a plane representing the screen surface.
	Vec3 fp;	// focal point behind the screen where you are sitting.
};


struct Light
{
	int Type;
	bool Enabled;
	Vec3 s;
	Colour<float> colour;
	Vec3 direction;
	float Cone;	// This is the cosine angle that light will spread away
	// from Dx, value 0-1
	float FuzFactor;// As percentage 0-1

	long LastPolyHit;	// This is for an optimization that was not used, it is being recorded.

	Light()
	{

	}

};

struct Vertex
{
	Vec3 l = { 0, 0, 0 };
	Vec3 w = { 0,0,0 };
};



struct Poly
{
	Vec3 n = { 0,0,0 };
	float D = 0.0f;
	z_size_t SurfaceTexture = 0;

	Vertex* vertex[3] = {};
	UV uv[3] = {};

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


struct Texture
{
	std::string filename;
	Colour<BYTE>* pixels_colour = NULL;
	Vec3* pixels_normal = NULL;
	unsigned char bmBitsPixel = 0;
	unsigned char bmBytesPixel = 0;
	uint16_t bmHeight = 0;
	long bmType = 0;
	uint16_t bmWidth = 0;
	uint16_t bmWidthBytes = 0;
	Colour<BYTE> mat_diffuse = { 0,0,0 };
	bool hasAlpha = 0;

	inline Colour<BYTE> get_pixel(z_screen_t x, z_screen_t y)
	{
		x %= bmWidth;
		y %= bmHeight;
		return pixels_colour[x + y * bmWidth];
	}

	inline Vec3 get_normal(z_screen_t x, z_screen_t y)
	{
		if (pixels_normal == NULL) return { 0,0,0 };
		x %= bmWidth;
		y %= bmHeight;
		return pixels_normal[x + y * bmWidth];
	}
};


struct Sprite
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
	z_size_t boTexture = 0;
};



struct ZSpace_Object
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
	Poly* polygon = nullptr;
	Vertex* vertex = nullptr;

	bool hidden = false;
};




struct ZSpace_World
{
	std::vector<std::unique_ptr<ZSpace_Object>> obj;
};





struct Clip
{
	Vec3 position = { 0,0,0 };
	float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
	float u = 0.0f, v = 0.0f;
	float u1 = 0.0f, v1 = 0.0f;
};

struct Clipped_Poly
{
	unsigned int vertcount = 0;
	Clip vertex[8] = {};
};

