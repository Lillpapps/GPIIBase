// Engine.cpp

#include "stdafx.h"
#include "InputSystem.h"
#include "RenderSystem.h"
#include "Engine.h"
#include "Camera.h"
#include "CameraKeyboard.h"
#include "CameraMouse.h"
#include "Plane.h"
#include "Box.h"
#include "AudioSystem.h"

Engine::Engine() {
	m_input_system = nullptr;
	m_render_system = nullptr;
	m_camera = nullptr;
	m_camera_keyboard = nullptr;
	m_camera_mouse = nullptr;
	m_audio_system = nullptr;
};

Engine::~Engine() {
};

bool Engine::Initialize(Config& config, void* window) {
	HWND hWnd = (HWND)window;
	int width = config.Get("width", 1024);
	int height = config.Get("height", 640);

	try {
		m_render_system = new RenderSystem(hWnd, width, height);
	}
	catch(std::exception e) {
		Debug::Msg(e.what());
		return false;
	};


	m_audio_system = new AudioSystem();

	m_input_system = new InputSystem;
	m_camera = new Camera(45.0f * 3.141592f / 180.0f, (float)width / (float)height, 0.5f, 1000.0f);
	m_camera_keyboard = new CameraKeyboard(m_camera);
	m_input_system->RegisterKeyboardListener(m_camera_keyboard);
	m_camera_mouse = new CameraMouse(m_camera);
	m_input_system->RegisterMouseListener(m_camera_mouse);

	m_box = new Box();
	if (!m_box->Intialize(m_render_system, m_camera)){
		return false;
	}

	m_plane = new Plane();
	if (!m_plane->Intialize(m_render_system, m_camera)){
		return false;
	}

	
	m_audio_system->Initialize(m_camera);
	m_audio_system->PlayMusic(m_audio_system->Attack1, 1.0f);
	
	
	return true;
};

void Engine::Cleanup() {
	if(m_input_system) {
		delete m_input_system;
		m_input_system = nullptr;
	};
	if(m_render_system) {
		delete m_render_system;	
		m_render_system = nullptr;
	};
	if (m_camera) {
		delete m_camera;
		m_camera = nullptr;
	};
	if (m_camera_keyboard) {
		delete m_camera_keyboard;
		m_camera_keyboard = nullptr;
	};
	if (m_camera_mouse) {
		delete m_camera_mouse;
		m_camera_mouse = nullptr;
	};
	if (m_plane) {
		delete m_plane;
		m_plane = nullptr;
	};
	if (m_box) {
		delete m_box;
		m_box = nullptr;
	};
};

bool Engine::Update(float deltatime) {
	
	m_camera->Update();
	m_audio_system->Update(m_camera);

	m_render_system->Clear();

	m_plane->Draw(m_render_system);
	m_render_system->SetShaderConstantMatrix("world", m_camera->GetView());	
	m_render_system->Apply();
	m_render_system->DrawIndexed(PRIMITIVE_TRIANGLES, 0, 6);
	
	m_box->Draw(m_render_system);
	m_render_system->SetShaderConstantMatrix("world", m_camera->GetView());	
	m_render_system->Apply();	
	m_render_system->DrawIndexed(PRIMITIVE_TRIANGLES, 0, 36);

	m_render_system->Present();
	
	
	return true;
};

void Engine::OnMouseMove(int x, int y) {
	m_input_system->OnMouseMove(x, y);
};

void Engine::OnMouseButton(int button, bool state) {
	m_input_system->OnMouseButton(button, state);
};

void Engine::OnKeyboard(int key, bool state) {
	m_input_system->OnKeyboard(key, state);
};

void Engine::OnFocus(bool state) {
};

void Engine::OnQuit() {
};
