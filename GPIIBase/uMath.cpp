// uMath.cpp

#include "stdafx.h"
#include <math.h>
#include "UMath.h"

static const float kPI = 3.14159f;
float Sqrt(float v) {return sqrtf(v);};
float Sin(float v) {return sinf(v);};
float Cos(float v) {return cosf(v);};
float Tan(float v) {return tanf(v);};
float ToRad(float v) {return v*kPI/180.0f;};
float ToDeg(float v) {return v*180.0f/kPI;};

static unsigned int seed = 1;
static unsigned int randi() {
	unsigned int hi, lo;
	lo=16807*(seed&0xFFFF);
	hi=16807*(seed>>16);
	lo+=(hi&0x7FFF)<<16;
	lo+=(hi>>15);
	if(lo>0x7FFFFFFF) lo-=0x7FFFFFFF;
	return seed = lo;
};

float Random(float min,float max) {
	return min + (max - min) * (float(randi()) / (float)0x7FFFFFFF);
};

Vec3::Vec3() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
};

Vec3::Vec3(float X, float Y,float Z) {
	x = X;
	y = Y;
	z = Z;
};

Vec3::Vec3(const Vec3 &rhs) {
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
};

Vec3 &Vec3::operator=(const Vec3 &rhs) {
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
};

Vec3 &Vec3::operator+=(const Vec3 &rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
};

Vec3 &Vec3::operator-=(const Vec3 &rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
};

Vec3 Vec3::operator+(const Vec3 &rhs) {
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
};

Vec3 Vec3::operator-(const Vec3 &rhs) {
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
};

Vec3 Vec3::operator*(const float rhs) {
	return Vec3(x * rhs, y * rhs, z * rhs);
};

Vec3 Vec3::operator/(const float rhs) {
	return Vec3(x / rhs, y / rhs, z / rhs);
};

void Vec3::Normalize() {
	float len = Length();
	if(len > 0.0f) {
		x /= len;
		y /= len;
		z /= len;
	};
};

float Vec3::Dot(const Vec3 &rhs) {
	return x * rhs.x + y * rhs.y + z * rhs.z;
};

float Vec3::Length() {
	return Sqrt(x * x + y * y + z * z);
};

Vec3 Vec3::Cross(const Vec3 &rhs) {
	return Vec3(
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x
	);
};

// -- BELOW: COPY AND PASTE -------------------------------------
Mat4::Mat4() {
	Identity();
};

Mat4::Mat4(const Mat4 &rhs) {
	memcpy(this, &rhs, sizeof(Mat4));
};

Mat4 &Mat4::operator=(const Mat4 &rhs) {
	memcpy(this, &rhs, sizeof(Mat4));
	return *this;
};

Mat4 Mat4::operator*(const Mat4 &rhs) {
	Mat4 r;
	r.m11 = m11*rhs.m11+m12*rhs.m21+m13*rhs.m31+m14*rhs.m41;
	r.m12 = m11*rhs.m12+m12*rhs.m22+m13*rhs.m32+m14*rhs.m42;
	r.m13 = m11*rhs.m13+m12*rhs.m23+m13*rhs.m33+m14*rhs.m43;
	r.m14 = m11*rhs.m14+m12*rhs.m24+m13*rhs.m34+m14*rhs.m44;

	r.m21 = m21*rhs.m11+m22*rhs.m21+m23*rhs.m31+m24*rhs.m41;
	r.m22 = m21*rhs.m12+m22*rhs.m22+m23*rhs.m32+m24*rhs.m42;
	r.m23 = m21*rhs.m13+m22*rhs.m23+m23*rhs.m33+m24*rhs.m43;
	r.m24 = m21*rhs.m14+m22*rhs.m24+m23*rhs.m34+m24*rhs.m44;

	r.m31 = m31*rhs.m11+m32*rhs.m21+m33*rhs.m31+m34*rhs.m41;
	r.m32 = m31*rhs.m12+m32*rhs.m22+m33*rhs.m32+m34*rhs.m42;
	r.m33 = m31*rhs.m13+m32*rhs.m23+m33*rhs.m33+m34*rhs.m43;
	r.m34 = m31*rhs.m14+m32*rhs.m24+m33*rhs.m34+m34*rhs.m44;

	r.m41 = m41*rhs.m11+m42*rhs.m21+m43*rhs.m31+m44*rhs.m41;
	r.m42 = m41*rhs.m12+m42*rhs.m22+m43*rhs.m32+m44*rhs.m42;
	r.m43 = m41*rhs.m13+m42*rhs.m23+m43*rhs.m33+m44*rhs.m43;
	r.m44 = m41*rhs.m14+m42*rhs.m24+m43*rhs.m34+m44*rhs.m44;
	return r;
};

Vec3 Mat4::operator*(const Vec3 &rhs) {
	return Vec3(rhs.x * m11 + rhs.y * m12 + rhs.z * m13, 
		rhs.x * m21 + rhs.y * m22 + rhs.z * m23, 
		rhs.x * m31 + rhs.y * m32 + rhs.z * m33);
};

void Mat4::Identity() {
	m12 = m13 = m14 = 0.0f;
	m21 = m23 = m24 = 0.0f;
	m31 = m32 = m34 = 0.0f;
	m41 = m42 = m43 = 0.0f;
	m11 = m22 = m33 = m44 = 1.0f;
};

void Mat4::OrthoOffCenter(Mat4 &o,float width,float height,float znear,float zfar) {
	o.Identity();
	float l=0.0f;
	float r=width;
	float b=height;
	float t=0.0f;
	o.m[0]=2.0f/(r-l);
	o.m[5]=2.0f/(t-b);
	o.m[10]=1.0f/(zfar-znear);
	o.m[12]=(l+r)/(l-r);
	o.m[13]=(t+b)/(b-t);
	o.m[14]=znear/(znear-zfar);
};

void Mat4::Perspective(Mat4 &o,float fov,float aspect,float znear,float zfar) {
	o.Identity();
	float h=1.0f/Tan(fov*0.5f);
	float w=h/aspect;
	o.m[0]=w;
	o.m[5]=h;
	o.m[10]=zfar/(zfar-znear);
	o.m[11]=1.0f;
	o.m[14]=(-znear*zfar)/(zfar-znear);
	o.m[15]=0.0f;
};

void Mat4::Translation(Mat4 &m,const Vec3 &v) {
	m.m12=m.m13=m.m14=0.0f;
	m.m21=m.m23=m.m24=0.0f;
	m.m31=m.m32=m.m34=0.0f;
	m.m11=m.m22=m.m33=m.m44=1.0f;
	m.m41=v.x;m.m42=v.y;m.m43=v.z;
};

void Mat4::Rotation(Mat4 &m,const Vec3 &axis,const float radians) {
	float c=Cos(radians);
	float s=Sin(radians);
	float xx=axis.x*axis.x;
	float xy=axis.x*axis.y;
	float xz=axis.x*axis.z;
	float yy=axis.y*axis.y;
	float yz=axis.y*axis.z;
	float zz=axis.z*axis.z;

	// build rotation Mat
	m.m[0]=xx*(1.0f-c)+c;
	m.m[1]=xy*(1.0f-c)-axis.z*s;
	m.m[2]=xz*(1.0f-c)+axis.y*s;
	m.m[3]=0;
	m.m[4]=xy*(1.0f-c)+axis.z*s;
	m.m[5]=yy*(1.0f-c)+c;		
	m.m[6]=yz*(1.0f-c)-axis.x*s;
	m.m[7]=0;
	m.m[8]=xz*(1.0f-c)-axis.y*s;
	m.m[9]=yz*(1.0f-c)+axis.x*s;
	m.m[10]=zz*(1.0f-c)+c;
	m.m[11]=0.0f;
	m.m[12]=0.0f;
	m.m[13]=0.0f;
	m.m[14]=0.0f;
	m.m[15]=1.0f;
};

void Mat4::Scaling(Mat4 &m,const Vec3 &v) {
	m.m12=m.m13=m.m14=0.0f;
	m.m21=m.m23=m.m24=0.0f;
	m.m31=m.m32=m.m34=0.0f;
	m.m41=m.m42=m.m43=0.0f;
	m.m44=1.0f;
	m.m11=v.x;m.m22=v.y;m.m33=v.z;
};
