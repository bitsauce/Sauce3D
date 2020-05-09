// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>

#include <Sauce/Input/Keycodes.h>
#include <Sauce/Input/Scancodes.h>

BEGIN_SAUCE_NAMESPACE

enum class InputButtonType : uint32
{
	None,
	Keyboard,
	Mouse,
	ControllerButton,
	ControllerAxis
};

class SAUCE_API InputButton
{
public:
	InputButton();
	InputButton(const Keycode keycode);
	InputButton(const Scancode scancode);
	InputButton(const MouseButton mouseButton);
	InputButton(const ControllerButton controllerButton);
	InputButton(const ControllerAxis axis);

	InputButton &operator=(const Scancode scancode);
	InputButton &operator=(const Keycode keycode);
	InputButton &operator=(const MouseButton mouseButton);
	InputButton &operator=(const ControllerButton controllerButton);
	InputButton &operator=(const ControllerAxis axis);

	bool operator==(const InputButton inputButton);
	bool operator==(const Scancode scancode);
	bool operator==(const Keycode keycode);
	bool operator==(const MouseButton mouseButton);
	bool operator==(const ControllerButton controllerButton);
	bool operator==(const ControllerAxis axis);

	InputButtonType getType() const;
	uint getCode() const;

private:
	uint code;
	InputButtonType type;
};

END_SAUCE_NAMESPACE