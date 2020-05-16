// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API Viewport
{
public:
	Viewport(const Rect<int32> &view);
	Viewport(const int x, const int y, const int w, const int h);

	void makeCurrent() const;

private:
	Rect<int32> m_viewport;
};

END_SAUCE_NAMESPACE
