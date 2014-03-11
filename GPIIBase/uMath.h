// uMath.h

#pragma once

float Sqrt(float v);
float Sin(float v) ;
float Cos(float v);
float Tan(float v);
float ToRad(float v);
float ToDeg(float v);

float Random(float min, float max);

template <class T> T Max(T a, T b) { return a < b ? b : a; };
template <class T> T Min(T a, T b) { return a > b ? b : a; };
template <class T> T Abs(T a) { return a < 0 ? -a : a; };
template <class T> T Clamp(T a, T m, T n) { return a < m ? m : a > n ? n : a; };

class Vec2 {
public:
	Vec2() {};
	Vec2(const Vec2& rhs) { x = rhs.x; y = rhs.y; };
	Vec2(float X, float Y) { x = X; y = Y; };
	float x, y;
};

class Vec3 {
public:
	Vec3();
	Vec3(float X, float Y,float Z);
	Vec3(const Vec3 &rhs);

	Vec3 &operator=(const Vec3 &rhs);
	Vec3 &operator+=(const Vec3 &rhs);
	Vec3 &operator-=(const Vec3 &rhs);
	Vec3 operator+(const Vec3 &rhs);
	Vec3 operator-(const Vec3 &rhs);
	Vec3 operator*(const float rhs);
	Vec3 operator/(const float rhs);

	void Normalize();
	float Dot(const Vec3 &rhs);
	float Length();
	Vec3 Cross(const Vec3 &rhs);

	float x,y,z;
};

class Vec4 {
public:
	Vec4() {};
	Vec4(const Vec4& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; };
	Vec4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };
	float x,y,z,w;
};

class Mat4 {
public:
	Mat4();
	Mat4(const Mat4 &rhs);

	Mat4 &operator=(const Mat4 &rhs);
	Mat4 operator*(const Mat4 &rhs);
	Vec3 operator*(const Vec3 &rhs);

	void Identity();

	static void OrthoOffCenter(Mat4 &o,float width,float height,float znear,float zfar);
	static void Perspective(Mat4 &o,float fov,float aspect,float znear,float zfar);
	static void Translation(Mat4 &m,const Vec3 &v);
	static void Rotation(Mat4 &m,const Vec3 &axis,const float radians);
	static void Scaling(Mat4 &m,const Vec3 &v);

	union {
		float m[16];
		struct {
			float m11,m12,m13,m14;
			float m21,m22,m23,m24;
			float m31,m32,m33,m34;
			float m41,m42,m43,m44;
		};
	};
};
