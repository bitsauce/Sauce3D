// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

enum class BlendFactor : uint32
{
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_SRC_COLOR,
	BLEND_ONE_MINUS_SRC_COLOR,
	BLEND_SRC_ALPHA,
	BLEND_ONE_MINUS_SRC_ALPHA,
	BLEND_DST_COLOR,
	BLEND_ONE_MINUS_DST_COLOR,
	BLEND_DST_ALPHA,
	BLEND_ONE_MINUS_DST_ALPHA,
	BLEND_SRC_ALPHA_SATURATE
};

enum class BlendPreset : uint32
{
	PRESET_ALPHA_BLEND,
	PRESET_OPAQUE,
	PRESET_ADDITIVE,
	PRESET_MULTIPLY,
	PRESET_PREMULTIPLIED_ALPHA
};

class SAUCE_API BlendState
{
	friend class GraphicsContext;
	friend class OpenGLContext;
public:

	BlendState(const BlendPreset preset);
	BlendState(const BlendFactor src, const BlendFactor dst);
	BlendState(const BlendFactor csrc, const BlendFactor cdst, const BlendFactor asrc, const BlendFactor adst);

private:
	BlendFactor m_src, m_dst, m_alphaSrc, m_alphaDst;
};

END_SAUCE_NAMESPACE
