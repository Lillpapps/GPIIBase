#include "stdafx.h"
#include "CameraKeyboard.h"
#include "Camera.h"

CameraKeyboard::CameraKeyboard(Camera* camera)
{
	m_camera = camera;
}


CameraKeyboard::~CameraKeyboard()
{
}

void CameraKeyboard::OnKey(int key, bool state)
{
	if (key == 65 && state){
		m_camera->Sidestep(-1.0f);
	}
	else if (key == 68 && state){
		m_camera->Sidestep(1.0f);
	}

	if (key == 87 && state){
		m_camera->Forward(1.0f);
	}
	else if (key == 83 && state){
		m_camera->Forward(-1.0f);
	}
}