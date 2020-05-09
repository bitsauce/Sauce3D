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
	: m_src(BlendFactor::Zero)
	, m_alphaSrc(BlendFactor::Zero)
	, m_dst(BlendFactor::Zero)
	, m_alphaDst(BlendFactor::Zero)
{
	switch(preset)
	{
	case BlendPreset::AlphaBlend:
		m_src = m_alphaSrc = BlendFactor::SrcAlpha;
		m_dst = m_alphaDst = BlendFactor::OneMinusSrcAlpha;
		break;
	case BlendPreset::Opaque:
	case BlendPreset::Additive:
		m_src = m_alphaSrc = BlendFactor::SrcAlpha;
		m_dst = m_alphaDst = BlendFactor::One;
		break;
	case BlendPreset::Multiply:
		m_src = m_alphaSrc = BlendFactor::DstColor;
		m_dst = m_alphaDst = BlendFactor::Zero;
		break;
	case BlendPreset::PremultipliedAlpha:
		m_src = m_alphaSrc = BlendFactor::One;
		m_dst = m_alphaDst = BlendFactor::OneMinusSrcAlpha;
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
