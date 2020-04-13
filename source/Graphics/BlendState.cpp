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

#include <Sauce/Graphics.h>

BEGIN_SAUCE_NAMESPACE

BlendState::BlendState(const BlendPreset preset)
	: m_src(BlendFactor::BLEND_ZERO)
	, m_alphaSrc(BlendFactor::BLEND_ZERO)
	, m_dst(BlendFactor::BLEND_ZERO)
	, m_alphaDst(BlendFactor::BLEND_ZERO)
{
	switch(preset)
	{
	case BlendPreset::PRESET_ALPHA_BLEND:
		m_src = m_alphaSrc = BlendFactor::BLEND_SRC_ALPHA;
		m_dst = m_alphaDst = BlendFactor::BLEND_ONE_MINUS_SRC_ALPHA;
		break;
	case BlendPreset::PRESET_OPAQUE:
	case BlendPreset::PRESET_ADDITIVE:
		m_src = m_alphaSrc = BlendFactor::BLEND_SRC_ALPHA;
		m_dst = m_alphaDst = BlendFactor::BLEND_ONE;
		break;
	case BlendPreset::PRESET_MULTIPLY:
		m_src = m_alphaSrc = BlendFactor::BLEND_DST_COLOR;
		m_dst = m_alphaDst = BlendFactor::BLEND_ZERO;
		break;
	case BlendPreset::PRESET_PREMULTIPLIED_ALPHA:
		m_src = m_alphaSrc = BlendFactor::BLEND_ONE;
		m_dst = m_alphaDst = BlendFactor::BLEND_ONE_MINUS_SRC_ALPHA;
		break;
	}
}

BlendState::BlendState(const BlendFactor src, const BlendFactor dst) :
	m_src(src),
	m_dst(dst),
	m_alphaSrc(src),
	m_alphaDst(dst)
{
}

BlendState::BlendState(const BlendFactor csrc, const BlendFactor cdst, const BlendFactor asrc, const BlendFactor adst) :
	m_src(csrc),
	m_dst(cdst),
	m_alphaSrc(asrc),
	m_alphaDst(adst)
{
}

END_SAUCE_NAMESPACE
