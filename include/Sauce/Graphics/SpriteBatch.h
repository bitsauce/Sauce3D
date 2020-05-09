// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Vertex.h>
#include <Sauce/Graphics/VertexBuffer.h>
#include <Sauce/Graphics/BlendState.h>
#include <Sauce/Graphics/Texture.h>
#include <Sauce/Graphics/Shader.h>
#include <Sauce/Graphics/FontRendering.h>

BEGIN_SAUCE_NAMESPACE

class Sprite;

/*********************************************************************
**	Batch															**
**********************************************************************/
class SAUCE_API SpriteBatch
{
public:
	SpriteBatch(const uint maxSprites = 2048);
	~SpriteBatch();

	enum class SpriteSortMode : uint32
	{
		BackToFront,
		Deferred,
		FrontToBack,
		Immediate,
		Texture
	};

	struct State
	{
		State(const SpriteSortMode mode=SpriteSortMode::Deferred, const BlendState blendState=BlendPreset::AlphaBlend, const Matrix4 &transformationMatix=Matrix4(), ShaderRef shader=nullptr) :
			mode(mode),
			blendState(blendState),
			transformationMatix(transformationMatix),
			shader(shader)
		{
		}

		SpriteSortMode mode;
		BlendState blendState;
		Matrix4 transformationMatix;
		ShaderRef shader;
	};

	void begin(GraphicsContext *graphicsContext, const State &state = State());
	void drawSprite(const Sprite &sprite);
	//void drawText(const Vector2F &pos, const string &text, Font *font); // TODO: Fonts
	void end();
	void flush();

	State getState() const { return m_state; }
	uint getTextureSwapCount() const;

private:

	// SpriteBatch state
	State m_state;

	// Vertex & index buffers
	Vertex *m_vertices;
	uint *m_indices;
	Sprite *m_sprites;
	uint m_spriteCount;
	const uint m_maxSpriteCount;
	GraphicsContext *m_graphicsContext;
};

END_SAUCE_NAMESPACE
