// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>

BEGIN_SAUCE_NAMESPACE

enum class BlendFactor : uint32
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstColor,
	OneMinusDstColor,
	DstAlpha,
	OneMinusDstAlpha,
	SrcAlphaSaturate
};

enum class BlendPreset : uint32
{
	AlphaBlend,
	Opaque,
	Additive,
	Multiply,
	PremultipliedAlpha
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
