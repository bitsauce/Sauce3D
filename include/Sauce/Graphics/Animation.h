// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/TextureRegion.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API SpriteAnimation
{
public:
	SpriteAnimation(const int nRows, const int nColumns);
	~SpriteAnimation();

	TextureRegion getKeyFrame(uint frameIndex);

private:
	vector<TextureRegion> m_textureRegions;
};

END_SAUCE_NAMESPACE
