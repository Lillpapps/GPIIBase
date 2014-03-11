#pragma once

#include "InputSystem.h"

class Camera;

class CameraMouse : public IMouseListener
{
public:
	CameraMouse(Camera* camera);
	~CameraMouse();
	void OnMove(int x, int y);
	void OnButton(int button, bool state);

private:
	float m_mouse_y;
	float m_mouse_x;
	int m_key;
	bool m_state;
	Camera* m_camera;
};

