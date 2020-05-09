//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Input/InputButton.h>

BEGIN_SAUCE_NAMESPACE

InputButton::InputButton() :
	code(0),
	type(InputButtonType::None)
{
}

InputButton::InputButton(const Keycode keycode) :
	code(SDL_GetScancodeFromKey((SDL_Keycode) keycode)),
	type(InputButtonType::Keyboard)
{
}

InputButton::InputButton(const Scancode scancode) :
	code(scancode),
	type(InputButtonType::Keyboard)
{
}

InputButton::InputButton(const MouseButton mouseButton) :
	code(mouseButton),
	type(InputButtonType::Mouse)
{
}

InputButton::InputButton(const ControllerButton controllerButton) :
	code(controllerButton),
	type(InputButtonType::ControllerButton)
{
}

InputButton::InputButton(const ControllerAxis axis) :
	code(axis),
	type(InputButtonType::ControllerAxis)
{
}

InputButton &InputButton::operator=(const Scancode scancode)
{
	code = scancode;
	type = InputButtonType::Keyboard;
	return *this;
}

InputButton &InputButton::operator=(const Keycode keycode)
{
	code = SDL_GetScancodeFromKey((SDL_Keycode) keycode);
	type = InputButtonType::Keyboard;
	return *this;
}

InputButton &InputButton::operator=(const MouseButton mouseButton)
{
	code = mouseButton;
	type = InputButtonType::Mouse;
	return *this;
}

InputButton &InputButton::operator=(const ControllerButton controllerButton)
{
	code = controllerButton;
	type = InputButtonType::ControllerButton;
	return *this;
}

InputButton &InputButton::operator=(const ControllerAxis axis)
{
	code = axis;
	type = InputButtonType::ControllerAxis;
	return *this;
}

bool InputButton::operator==(const InputButton inputButton)
{
	return type == inputButton.type && code == inputButton.code;
}

bool InputButton::operator==(const Scancode scancode)
{
	return type == InputButtonType::Keyboard && code == scancode;
}

bool InputButton::operator==(const Keycode keycode)
{
	return type == InputButtonType::Keyboard && code == SDL_GetScancodeFromKey((SDL_Keycode) keycode);
}

bool InputButton::operator==(const MouseButton mouseButton)
{
	return type == InputButtonType::Mouse && code == mouseButton;
}

bool InputButton::operator==(const ControllerButton controllerButton)
{
	return type == InputButtonType::ControllerButton && code == controllerButton;
}

bool InputButton::operator==(const ControllerAxis axis)
{
	return type == InputButtonType::ControllerAxis && code == axis;
}

InputButtonType InputButton::getType() const
{
	return type;
}

uint InputButton::getCode() const
{
	return code;
}

END_SAUCE_NAMESPACE