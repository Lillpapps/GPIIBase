// Camera.cpp

#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float fov, float aspect, float znear, float zfar)
: m_forward(0.0f, 0.0f, 0.0f)
, m_right(0.0f, 0.0f, 1.0f)
, m_up(0.0f, 1.0f, 0.0)
, m_position(0.0f, 0.0f, -10.0f)
{
	m_dirty = true;
	m_yaw = m_pitch = m_roll = 0.0f;
	m_view.Identity();
	SetPerspective(fov, aspect, znear, zfar);
	m_delta = 0.0f;
};

void Camera::Update() {
	m_delta = m_timer.Update();
	if(!m_dirty) return;
	m_dirty = false;

	Vec3 right(1.0f, 0.0f, 0.0);
	Vec3 up(0.0f, 1.0f, 0.0f);
	Vec3 forward(0.0f, 0.0f, 1.0f);

	Mat4 ry;
	Mat4::Rotation(ry, up, m_yaw);
	right = ry * right;
	forward = ry * forward;

	Mat4 rx;
	Mat4::Rotation(rx, right, m_pitch);
	up = rx * up;
	forward = rx * forward;

	Mat4 rz;
	Mat4::Rotation(rz, Vec3(1.0f, 0.0f, 0.0f), m_roll);

	right.Normalize();
	up.Normalize();
	forward.Normalize();

	m_right = right;
	m_up = up;
	m_forward = forward;

	// construct view matrix
	m_view.m11 = m_right.x; m_view.m12 = m_up.x; m_view.m13 = m_forward.x; m_view.m14 = 0.0f;
	m_view.m21 = m_right.y; m_view.m22 = m_up.y; m_view.m23 = m_forward.y; m_view.m24 = 0.0f;
	m_view.m31 = m_right.z; m_view.m32 = m_up.z; m_view.m33 = m_forward.z; m_view.m34 = 0.0f;
	m_view.m41 = -m_position.Dot(m_right); 
	m_view.m42 = -m_position.Dot(m_up); 
	m_view.m43 = -m_position.Dot(m_forward); 
	m_view.m44 = 1.0f;
};

void Camera::Forward(float dir) {
	m_dirty = true;
	m_position += m_forward * dir * (20.0f * m_delta);
};

void Camera::Sidestep(float dir) {
	m_dirty = true;
	m_position += m_right * dir * (20.0f * m_delta);
};

void Camera::Elevate(float dir) {
	m_dirty = true;
	m_position += m_up * dir * (20.0f * m_delta);
};

void Camera::RotateX(float amount) {
	m_dirty = true;
	m_pitch += (amount * m_delta);
};

void Camera::RotateY(float amount) {
	m_dirty = true;
	m_yaw += (amount * m_delta);
};

void Camera::RotateZ(float amount) {
	m_dirty = true;
	m_roll += (amount * m_delta);
};

void Camera::SetPerspective(float fov, float aspect, float znear, float zfar) {
	Mat4::Perspective(m_proj, fov, aspect, znear, zfar);
};
