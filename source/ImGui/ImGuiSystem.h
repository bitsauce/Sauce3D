// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>
#include <Sauce/ImGui.h>

BEGIN_SAUCE_NAMESPACE

// TODO: Better name
class ImGuiSystem
{
public:
	static void initialize(void* hwnd);
	static void processInputs(const float deltaTime, const char textInputChar);
	static void newFrame();
	static void render();
};

END_SAUCE_NAMESPACE
