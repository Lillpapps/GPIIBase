// Engine.h

#pragma once

class InputSystem;
class RenderSystem;
class Camera;
class CameraKeyboard;
class CameraMouse;
class Plane;
class Box;

class Engine {
public:
	Engine();
	~Engine();

	bool Initialize(Config& config, void* window);
	void Cleanup();
	bool Update(float deltatime);

	void OnMouseMove(int x, int y);
	void OnMouseButton(int button, bool state);
	void OnKeyboard(int key, bool state);
	void OnFocus(bool state);
	void OnQuit();

private:
	InputSystem* m_input_system;
	RenderSystem* m_render_system;
	Camera* m_camera;
	CameraKeyboard* m_camera_keyboard;
	CameraMouse* m_camera_mouse;
	Plane* m_plane;
	Box* m_box;
};
