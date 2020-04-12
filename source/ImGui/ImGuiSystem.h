#pragma once

#include <Sauce/Config.h>
#include <Sauce/ImGui/imgui_library.h>

BEGIN_SAUCE_NAMESPACE

// TODO: Better name
class ImGuiSystem
{
public:
	static void initialize(void* hwnd);
	static void processInputs(const float deltaTime);
	static void newFrame();
	static void render();
};

END_SAUCE_NAMESPACE
