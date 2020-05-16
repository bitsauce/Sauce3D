// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/Input/Keycodes.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

/*********************************************************************
**	Event types														**
**********************************************************************/
#define START_EVENT_ENUM _StartEventID = (uint32)LAST_DEFINED_EVENT_ENUM::_MaxEventID
#define END_EVENT_ENUM _MaxEventID

enum class InvalidEventType : int32
{
	Invalid     = -1,
	_MaxEventID = 0
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM InvalidEventType

enum class CoreEventType : int32
{
	START_EVENT_ENUM,
	Tick,
	Draw,
	TextInput,
	ControllerAxis,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM CoreEventType

enum class WindowEventType : int32
{
	START_EVENT_ENUM,
	SizeChanged,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM WindowEventType

enum class GameEventType : int32
{
	START_EVENT_ENUM,
	Start,
	End,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM GameEventType

enum class StepEventType : int32
{
	START_EVENT_ENUM,
	Begin,
	End,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM StepEventType

enum class KeyEventType : int32
{
	START_EVENT_ENUM,
	Down,
	Up,
	Repeat,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM KeyEventType

enum class MouseEventType : int32
{
	START_EVENT_ENUM,
	Move,
	Down,
	Up,
	Wheel,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM MouseEventType

enum class ControllerButtonEventType : int32
{
	START_EVENT_ENUM,
	Down,
	Up,
	Repeat,
	END_EVENT_ENUM
};
#undef LAST_DEFINED_EVENT_ENUM
#define LAST_DEFINED_EVENT_ENUM ControllerButtonEventType

class SAUCE_API Event
{
protected:
	Event(const int32 type) :
		m_type(type),
		m_userData(0)
	{
	}

public:
	int32 getType() const
	{
		return m_type;
	}

	void *getUserData() const
	{
		return m_userData;
	}

	void setUserData(void *data)
	{
		m_userData = data;
	}

private:
	const int32 m_type;
	void*       m_userData;
};

class SAUCE_API TextEvent : public Event
{
public:
	TextEvent(const char c) :
		Event((int32)CoreEventType::TextInput),
		m_char(c)
	{
	}

	char getChar() const
	{
		return m_char;
	}

private:
	const char m_char;
};

class SAUCE_API DrawEvent : public Event
{
public:
	DrawEvent(const float alpha, const float delta, GraphicsContextRef graphicsContext) :
		Event((int32)CoreEventType::Draw),
		m_alpha(alpha),
		m_delta(delta),
		m_graphicsContext(graphicsContext)
	{
	}

	float getAlpha() const
	{
		return m_alpha;
	}

	float getDelta() const
	{
		return m_delta;
	}

	GraphicsContextRef getGraphicsContext() const
	{
		return m_graphicsContext;
	}

private:
	const float m_alpha, m_delta;
	GraphicsContextRef m_graphicsContext;
};

class SAUCE_API TickEvent : public Event
{
public:
	TickEvent(const float delta) :
		Event((int32)CoreEventType::Tick),
		m_delta(delta)
	{
	}

	float getDelta() const
	{
		return m_delta;
	}

private:
	const float m_delta;
};

class SAUCE_API WindowEvent : public Event
{
public:
	WindowEvent(const WindowEventType type, class Window *window, const int32 width, const int32 height) :
		Event((int32)type),
		m_window(window),
		m_width(width),
		m_height(height)
	{
	}

	class Window *getWindow() const
	{
		return m_window;
	}

	int32 getWidth() const
	{
		return m_width;
	}

	int32 getHeight() const
	{
		return m_height;
	}

private:
	class Window *m_window;
	int32 m_width, m_height;
};

class SAUCE_API GameEvent : public Event
{
public:
	GameEvent(const GameEventType type) :
		Event((int32)type)
	{
	}
};

class SAUCE_API StepEvent : public Event
{
public:
	StepEvent(const StepEventType type) :
		Event((int32)type)
	{
	}
};

class SAUCE_API InputEvent : public Event
{
protected:
	InputEvent(const uint type, InputManager *inputManager, const InputButton button) :
		Event((int32)type),
		m_inputManager(inputManager),
		m_inputButton(button)
	{
	}

	InputEvent(const uint type, InputManager *inputManager) :
		Event((int32)type),
		m_inputManager(inputManager),
		m_inputButton()
	{
	}

public:
	InputManager *getInputManager() const
	{
		return m_inputManager;
	}

	InputButton getInputButton() const
	{
		return m_inputButton;
	}

protected:
	InputManager * const m_inputManager;
	const InputButton m_inputButton;
};

class SAUCE_API KeyEvent : public InputEvent
{
public:
	KeyEvent(const KeyEventType type, InputManager *inputManager, const InputButton inputButton, const uint16 modifiers) :
		InputEvent((int32)type, inputManager, inputButton),
		m_modifiers(modifiers)
	{
	}

	Keycode getKeycode() const;

	Scancode getScancode() const
	{
		return (Scancode) m_inputButton.getCode();
	}

	uint16 getModifiers() const
	{
		return m_modifiers;
	}

private:
	const uint16 m_modifiers;
};

class SAUCE_API MouseEvent : public InputEvent
{
public:
	MouseEvent(const MouseEventType type, InputManager *inputManager, const int32 x, const int32 y, const MouseButton button, const int32 wheelX, const int32 wheelY) :
		InputEvent((int32)type, inputManager, button),
		m_x(x),
		m_y(y),
		m_wheelX(wheelX),
		m_wheelY(wheelY)
	{
	}

	MouseButton getButton() const
	{
		return (MouseButton) getInputButton().getCode();
	}

	int32 getX() const
	{
		return m_x;
	}

	int32 getY() const
	{
		return m_y;
	}

	Vector2<int32> getPosition() const
	{
		return Vector2<int32>(m_x, m_y);
	}

	int32 getWheelX() const
	{
		return m_wheelX;
	}

	int32 getWheelY() const
	{
		return m_wheelY;
	}

private:
	const int32 m_x, m_y;
	const int32 m_wheelX, m_wheelY;
};

class SAUCE_API ControllerButtonEvent : public InputEvent
{
public:
	ControllerButtonEvent(const ControllerButtonEventType type, InputManager *inputManager, const ControllerButton controllerButton) :
		InputEvent((int32)type, inputManager, controllerButton)
	{
	}
};

class SAUCE_API ControllerAxisEvent : public InputEvent
{
public:
	ControllerAxisEvent(InputManager *inputManager, const ControllerAxis axis, const short value) :
		InputEvent((int32)CoreEventType::ControllerAxis, inputManager),
		m_axis(axis),
		m_value(value)
	{
	}

	ControllerAxis getAxis() const { return m_axis; }
	short getValue() const { return m_value; }

private:
	const ControllerAxis m_axis;
	const short m_value;
};

END_SAUCE_NAMESPACE
