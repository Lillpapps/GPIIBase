#pragma once

#include "InputSystem.h"

class Camera;

class CameraKeyboard : public IKeyboardListener
{
public:
	CameraKeyboard(Camera* camera);
	~CameraKeyboard();
	void OnKey(int key, bool state);

private:
	Camera* m_camera;
};

