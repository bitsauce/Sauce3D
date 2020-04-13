// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API TextureRegion
{
public:
	TextureRegion();
	TextureRegion(const Vector2F &uv0, const Vector2F &uv1);
	TextureRegion(const float u0, const float v0, const float u1, const float v1);
	TextureRegion(const TextureRegion &other);
	~TextureRegion();

	TextureRegion &operator=(const TextureRegion &other);

	void setRegion(const Vector2F &uv0, const Vector2F &uv1);
	void setRegion(const float u0, const float v0, const float u1, const float v1);

	//TextureRegion getSubRegion(const Vector2F &uv0, const Vector2F &uv1);
	//TextureRegion getSubRegion(const float u0, const float v0, const float u1, const float v1);

	Vector2F uv0;
	Vector2F uv1;
};

END_SAUCE_NAMESPACE
