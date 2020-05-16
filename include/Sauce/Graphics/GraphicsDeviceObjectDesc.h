// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

struct SAUCE_API GraphicsDeviceObjectDesc : public SauceObjectDesc
{
	GraphicsContextRef graphicsContext = nullptr;
	string             debugName       = "";
};

END_SAUCE_NAMESPACE
