// InputSystem.h

#pragma once

class IKeyboardListener {
public:
	virtual ~IKeyboardListener() {};
	virtual void OnKey(int key, bool state) = 0;
};

class IMouseListener {
public:
	virtual ~IMouseListener() {};
	virtual void OnMove(int x, int y) = 0;
	virtual void OnButton(int button, bool state) = 0;
};

class InputSystem {
public:
	InputSystem();
	~InputSystem();

	void OnMouseMove(int x, int y);
	void OnMouseButton(int button, bool state);
	void OnKeyboard(int key, bool state);

	void RegisterKeyboardListener(IKeyboardListener* listener);
	void UnregisterKeyboardListener(IKeyboardListener* listener);
	void RegisterMouseListener(IMouseListener* listener);
	void UnregisterMouseListener(IMouseListener* listener);

private:
	std::vector<IKeyboardListener*> m_keyboard;
	std::vector<IMouseListener*> m_mouse;
};
