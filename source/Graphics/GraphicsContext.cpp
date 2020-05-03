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

// Default shader. Used when no shader is set.
ShaderRef GraphicsContext::s_defaultShader = 0;

// Default texture. Empty texture used when no texture is set.
Texture2DRef GraphicsContext::s_defaultTexture = 0;

// Singleton pointer
GraphicsContext* GraphicsContext::s_this = nullptr;

Vertex *GraphicsContext::getVertices(const uint vertexCount)
{
	if(vertexCount > m_vertices.size())
	{
		m_vertices.resize(vertexCount);
	}
	return &m_vertices[0];
}

GraphicsContext::GraphicsContext()
	: m_context(nullptr)
	, m_window(nullptr)
{
	assert(s_this == nullptr);
	s_this = this;

	State state;
	m_stateStack.push(state);
	m_currentState = &m_stateStack.top();
}

GraphicsContext::~GraphicsContext()
{
}

GraphicsContext* GraphicsContext::GetContext()
{
	return s_this;
}

void GraphicsContext::pushRenderTarget(RenderTarget2DRef renderTarget)
{
	// Unbind previous render target
	if(m_currentState->renderTarget)
	{
		// TODO: Is it necessary to unbind here every time?
		renderTarget2D_bindRenderTarget(nullptr);
	}

	// Push state
	pushState();

	// Bind render target
	{
		renderTarget2D_bindRenderTarget(renderTarget->m_deviceObject);
		m_currentState->renderTarget = renderTarget;
	}

	// Resize viewport
	Texture2DRef targetTexture = renderTarget->getTargetTexture();
	setProjectionMatrix(createOrtographicMatrix(0, targetTexture->getWidth(), targetTexture->getHeight(), 0)); // TODO: Maybe this shouldn't be here?
	setSize(targetTexture->getWidth(), targetTexture->getHeight());
}

void GraphicsContext::popRenderTarget()
{
	// Unbind previous render target
	if(m_currentState->renderTarget)
	{
		// TODO: Is it necessary to unbind here every time?
		renderTarget2D_bindRenderTarget(nullptr);
	}

	// Pop state
	popState();

	// Resize viewport
	if(m_currentState->renderTarget)
	{
		// Bind render target
		RenderTarget2DRef renderTarget = m_currentState->renderTarget;
		renderTarget2D_bindRenderTarget(renderTarget->m_deviceObject);

		// Resize viewport
		Texture2DRef targetTexture = renderTarget->getTargetTexture();
		setProjectionMatrix(createOrtographicMatrix(0, targetTexture->getWidth(), targetTexture->getHeight(), 0)); // TODO: Maybe this shouldn't be here?
		setSize(targetTexture->getWidth(), targetTexture->getHeight());
	}
	else
	{
		setSize(m_window->getWidth(), m_window->getHeight());
	}
}

void GraphicsContext::pushState()
{
	m_stateStack.push(*m_currentState);
	m_currentState = &m_stateStack.top();
}

void GraphicsContext::popState()
{
	m_stateStack.pop();
	if(m_stateStack.empty()) THROW("GraphicsContext: State stack should not be empty.");
	m_currentState = &m_stateStack.top();
}

void GraphicsContext::pushMatrix(const Matrix4 &mat)
{
	m_currentState->transformationMatrixStack.push(m_currentState->transformationMatrixStack.top() * mat);
}

bool GraphicsContext::popMatrix()
{
	if(m_currentState->transformationMatrixStack.size() > 1)
	{
		m_currentState->transformationMatrixStack.pop();
		return true;
	}
	return false;
}

Matrix4 GraphicsContext::topMatrix() const
{
	if(m_currentState->transformationMatrixStack.empty()) return Matrix4();
	return m_currentState->transformationMatrixStack.top();
}

void GraphicsContext::clearMatrixStack()
{
	while(popMatrix());
}

void GraphicsContext::setTexture(Texture2DRef texture)
{
	m_currentState->texture = texture;
}

Texture2DRef GraphicsContext::getTexture() const
{
	return m_currentState->texture;
}

void GraphicsContext::setShader(ShaderRef shader)
{
	m_currentState->shader = shader;
}

ShaderRef GraphicsContext::getShader() const
{
	return m_currentState->shader;
}

void GraphicsContext::setBlendState(const BlendState &blendState)
{
	m_currentState->blendState = blendState;
}

BlendState GraphicsContext::getBlendState()
{
	return m_currentState->blendState;
}

// Orthographic projection
void GraphicsContext::setSize(const uint w, const uint h)
{
	// Set size
	m_currentState->width = w;
	m_currentState->height = h;

	// Set viewport
	setViewportSize(w, h);
}

void GraphicsContext::setProjectionMatrix(const Matrix4 matrix)
{
	m_currentState->projectionMatrix = matrix;
}

void GraphicsContext::drawRectangle(const float x, const float y, const float width, const float height, const Color &color, const TextureRegion &textureRegion)
{
	// Make sure we have enough vertices
	if(m_vertices.size() < 4)
	{
		m_vertices.resize(4);
	}

	m_vertices[0].set2f(VertexAttribute::VERTEX_POSITION, x, y);
	m_vertices[1].set2f(VertexAttribute::VERTEX_POSITION, x, y + height);
	m_vertices[2].set2f(VertexAttribute::VERTEX_POSITION, x + width, y);
	m_vertices[3].set2f(VertexAttribute::VERTEX_POSITION, x + width, y + height);

	m_vertices[0].set4ub(VertexAttribute::VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
	m_vertices[1].set4ub(VertexAttribute::VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
	m_vertices[2].set4ub(VertexAttribute::VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
	m_vertices[3].set4ub(VertexAttribute::VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());

	m_vertices[0].set2f(VertexAttribute::VERTEX_TEX_COORD, textureRegion.uv0.x, textureRegion.uv0.y);
	m_vertices[1].set2f(VertexAttribute::VERTEX_TEX_COORD, textureRegion.uv0.x, textureRegion.uv1.y);
	m_vertices[2].set2f(VertexAttribute::VERTEX_TEX_COORD, textureRegion.uv1.x, textureRegion.uv0.y);
	m_vertices[3].set2f(VertexAttribute::VERTEX_TEX_COORD, textureRegion.uv1.x, textureRegion.uv1.y);

	drawPrimitives(PrimitiveType::PRIMITIVE_TRIANGLE_STRIP, &m_vertices[0], 4);
}

void GraphicsContext::drawRectangle(const Vector2F &pos, const Vector2F &size, const Color &color, const TextureRegion &textureRegion)
{
	drawRectangle(pos.x, pos.y, size.x, size.y, color, textureRegion);
}

void GraphicsContext::drawRectangle(const Rect<float> &rect, const Color &color, const TextureRegion &textureRegion)
{
	drawRectangle(rect.position.x, rect.position.y, rect.size.x, rect.size.y, color, textureRegion);
}

void GraphicsContext::drawRectangleOutline(const float x, const float y, const float width, const float height, const Color &color, const TextureRegion &textureRegion)
{
	// Make sure we have enough vertices
	if(m_vertices.size() < 8)
	{
		m_vertices.resize(8);
	}

	m_vertices[0].set2f(VertexAttribute::VERTEX_POSITION, x, y);
	m_vertices[1].set2f(VertexAttribute::VERTEX_POSITION, x, y + height);

	m_vertices[2].set2f(VertexAttribute::VERTEX_POSITION, x, y + height);
	m_vertices[3].set2f(VertexAttribute::VERTEX_POSITION, x + width, y + height);
	
	m_vertices[4].set2f(VertexAttribute::VERTEX_POSITION, x + width, y + height);
	m_vertices[5].set2f(VertexAttribute::VERTEX_POSITION, x + width, y);

	m_vertices[6].set2f(VertexAttribute::VERTEX_POSITION, x + width, y);
	m_vertices[7].set2f(VertexAttribute::VERTEX_POSITION, x, y);

	for(int i = 0; i < 8; i++)
	{
		m_vertices[i].set4ub(VertexAttribute::VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
		m_vertices[i].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.0f, 0.0f);
	}

	drawPrimitives(PrimitiveType::PRIMITIVE_LINES, &m_vertices[0], 8);
}

void GraphicsContext::drawRectangleOutline(const Vector2F &pos, const Vector2F &size, const Color &color, const TextureRegion &textureRegion)
{
	drawRectangleOutline(pos.x, pos.y, size.x, size.y, color, textureRegion);
}

void GraphicsContext::drawRectangleOutline(const Rect<float> &rect, const Color &color, const TextureRegion &textureRegion)
{
	drawRectangleOutline(rect.position.x, rect.position.y, rect.size.x, rect.size.y, color, textureRegion);
}

void GraphicsContext::drawCircleGradient(const float x, const float y, const float radius, const uint segments, const Color &center, const Color &outer)
{
	// Make sure we have enough vertices
	if(m_vertices.size() < segments + 2)
	{
		m_vertices.resize(segments + 2);
	}

	m_vertices[0].set2f(VertexAttribute::VERTEX_POSITION, x, y);
	m_vertices[0].set4ub(VertexAttribute::VERTEX_COLOR, center.getR(), center.getG(), center.getB(), center.getA());
	m_vertices[0].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.5f, 0.5f);

	for(uint i = 1; i < segments + 2; ++i)
	{
		float r = (2.0f * PI * i) / segments;
		m_vertices[i].set2f(VertexAttribute::VERTEX_POSITION, x + cos(r) * radius, y + sin(r) * radius);
		m_vertices[i].set4ub(VertexAttribute::VERTEX_COLOR, outer.getR(), outer.getG(), outer.getB(), outer.getA());
		m_vertices[i].set2f(VertexAttribute::VERTEX_TEX_COORD, (1.0f + cos(r)) / 2.0f, (1.0f + sin(r)) / 2.0f);
	}

	drawPrimitives(PrimitiveType::PRIMITIVE_TRIANGLE_FAN, &m_vertices[0], segments + 2);
}

void GraphicsContext::drawCircleGradient(const Vector2F &pos, const float radius, const uint segments, const Color &center, const Color &outer)
{
	drawCircleGradient(pos.x, pos.y, radius, segments, center, outer);
}

void GraphicsContext::drawCircle(const float x, const float y, const float radius, const uint segments, const Color &color)
{
	drawCircleGradient(x, y, radius, segments, color, color);
}

void GraphicsContext::drawCircle(const Vector2F &pos, const float radius, const uint segments, const Color &color)
{
	drawCircleGradient(pos.x, pos.y, radius, segments, color, color);
}

void GraphicsContext::drawArrow(const float x0, const float y0, const float x1, const float y1, const float arrowHeadSize, const Color &color)
{
	// Make sure we have enough vertices
	if(m_vertices.size() < 6) m_vertices.resize(6);

	m_vertices[0].set2f(VertexAttribute::VERTEX_POSITION, x0, y0); m_vertices[0].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[0].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);
	m_vertices[1].set2f(VertexAttribute::VERTEX_POSITION, x1, y1); m_vertices[1].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[1].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);

	Vector2F p0 = (Vector2F(x0, y0) - Vector2F(x1, y1)).normalized() * arrowHeadSize;
	Vector2F p1 = p0;
	const float angle = math::degToRad(30.f);
	p0.rotate(-angle);
	p1.rotate(angle);

	p0 += Vector2F(x1, y1);
	p1 += Vector2F(x1, y1);

	m_vertices[2].set2f(VertexAttribute::VERTEX_POSITION, x1, y1); m_vertices[2].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[2].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);
	m_vertices[3].set2f(VertexAttribute::VERTEX_POSITION, p0.x, p0.y); m_vertices[3].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[3].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);

	m_vertices[4].set2f(VertexAttribute::VERTEX_POSITION, x1, y1); m_vertices[4].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[4].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);
	m_vertices[5].set2f(VertexAttribute::VERTEX_POSITION, p1.x, p1.y); m_vertices[5].set4ub(VertexAttribute::VERTEX_COLOR, color.x, color.y, color.z, color.w); m_vertices[5].set2f(VertexAttribute::VERTEX_TEX_COORD, 0.f, 0.f);

	drawPrimitives(PrimitiveType::PRIMITIVE_LINES, &m_vertices[0], 6);
}

void GraphicsContext::texture2D_getDeviceObject(Texture2DRef texture, Texture2DDeviceObject*& outTextureDeviceObject)
{
	outTextureDeviceObject = texture->m_deviceObject;
}

void GraphicsContext::shader_getDeviceObject(ShaderRef shader, ShaderDeviceObject*& outShaderDeviceObject)
{
	outShaderDeviceObject = shader->m_deviceObject;
}

void GraphicsContext::renderTarget2D_getDeviceObject(RenderTarget2DRef renderTarget, RenderTarget2DDeviceObject*& outRenderTargetDeviceObject)
{
	outRenderTargetDeviceObject = renderTarget->m_deviceObject;
}

void GraphicsContext::vertexBuffer_getDeviceObject(VertexBufferRef vertexBuffer, VertexBufferDeviceObject*& outVertexBufferDeviceObject)
{
	outVertexBufferDeviceObject = vertexBuffer->m_deviceObject;
}

void GraphicsContext::indexBuffer_getDeviceObject(IndexBufferRef indexBuffer, IndexBufferDeviceObject*& outIndexBufferDeviceObject)
{
	outIndexBufferDeviceObject = indexBuffer->m_deviceObject;
}

END_SAUCE_NAMESPACE

