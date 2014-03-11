#include "stdafx.h"
#include "CameraMouse.h"
#include "Camera.h"

CameraMouse::CameraMouse(Camera* camera)
{
	m_camera = camera;
	m_mouse_x = 0.0f;
	m_mouse_y = 0.0f;
	m_key = 0;
	m_state = false;
}


CameraMouse::~CameraMouse()
{
}

void CameraMouse::OnMove(int x, int y)
{
	float m_mouse_dx = x - m_mouse_x;
	float m_mouse_dy = y - m_mouse_y;
	if (m_key == 1 && m_state){
		m_camera->RotateY(m_mouse_dx);
		m_camera->RotateX(m_mouse_dy);
	}
	m_mouse_x = x;
	m_mouse_y = y;
	
}

void CameraMouse::OnButton(int key, bool state)
{
	m_key = key;
	m_state = state;
}
