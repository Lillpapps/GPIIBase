// InputSystem.cpp

#include "stdafx.h"
#include "InputSystem.h"

InputSystem::InputSystem() {
};

InputSystem::~InputSystem() {
	m_keyboard.clear();
	m_mouse.clear();
};

void InputSystem::OnMouseMove(int x, int y) {
	auto it = m_mouse.begin();
	while(it != m_mouse.end()) {
		(*it)->OnMove(x, y);
		++it;
	};
};

void InputSystem::OnMouseButton(int button, bool state) {
	auto it = m_mouse.begin();
	while(it != m_mouse.end()) {
		(*it)->OnButton(button, state);
		++it;
	};
};

void InputSystem::OnKeyboard(int key, bool state) {
	auto it = m_keyboard.begin();
	while(it != m_keyboard.end()) {
		(*it)->OnKey(key, state);
		++it;
	};
};

void InputSystem::RegisterKeyboardListener(IKeyboardListener* listener) {
	m_keyboard.push_back(listener);
};

void InputSystem::UnregisterKeyboardListener(IKeyboardListener* listener) {
	auto it = m_keyboard.begin();
	while(it != m_keyboard.end()) {
		if((*it) == listener) {
			m_keyboard.erase(it);
			break;
		};
		++it;
	};
};

void InputSystem::RegisterMouseListener(IMouseListener* listener) {
	m_mouse.push_back(listener);
};

void InputSystem::UnregisterMouseListener(IMouseListener* listener) {
	auto it = m_mouse.begin();
	while(it != m_mouse.end()) {
		if((*it) == listener) {
			m_mouse.erase(it);
			break;
		};
		++it;
	};
};
