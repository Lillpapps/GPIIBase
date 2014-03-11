// Camera.h

#pragma once

class Camera {
public:
	Camera(float fov, float aspect, float znear, float zfar);

	void Update();

	void Forward(float dir);
	void Sidestep(float dir);
	void Elevate(float dir);
	void RotateX(float dir);
	void RotateY(float dir);
	void RotateZ(float dir);

	void SetPerspective(float fov, float aspect, float znear, float zfar);

	Mat4& GetProj() { return m_proj; };
	Mat4& GetView() { return m_view; };
	Vec3 GetPosition() const { return m_position; };

private:
	bool m_dirty;

	float m_yaw;
	float m_pitch;
	float m_roll;

	Vec3 m_right;
	Vec3 m_up;
	Vec3 m_forward;
	Vec3 m_position;

	Mat4 m_proj;
	Mat4 m_view;

	float m_delta;
	Timer m_timer;
};