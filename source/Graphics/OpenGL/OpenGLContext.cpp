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

#include <GL/gl3w.h>
#include <GL/wglext.h>

#pragma warning(disable: 6011) // Ignore "Dereferencing NULL pointer" warning

BEGIN_SAUCE_NAMESPACE

/**************************************************
 * Globals                                        *
 **************************************************/

/** Global buffer objects to make rendering easier */
GLuint g_vao = 0;
GLuint g_vbo = 0;
GLuint g_ibo = 0;

/** Stored max texture size */
GLint g_maxTextureSize = -1;

/** Zeroed texture data (maxSize x maxSize) */
GLubyte* g_zeroedTextureDataArray = nullptr;

/** Stored GLSL version string (inserted into top of shader files) */
string s_glslVersion;

/**************************************************
 * Device object definitions                      *
 **************************************************/
struct OpenGLTexture2DDeviceObject : public Texture2DDeviceObject
{
	GLuint id = 0;
};

struct ShaderUniform
{
	ShaderUniform() :
		type(0),
		loc(0),
		count(0),
		data(nullptr),
		texture(nullptr)
	{
	}

	~ShaderUniform()
	{
		delete[] data;
	}

	GLenum type;
	int loc;
	int count;
	void* data;
	Texture2DRef texture;
};

struct OpenGLShaderDeviceObject : public ShaderDeviceObject
{
	GLuint id = 0;
	unordered_map<string, ShaderUniform*> uniforms = unordered_map<string, ShaderUniform*>();
};

struct OpenGLRenderTarget2DDeviceObject : public RenderTarget2DDeviceObject
{
	GLuint id = 0;
};

struct OpenGLVertexBufferDeviceObject : public VertexBufferDeviceObject
{
	GLuint id = 0;
};

struct OpenGLIndexBufferDeviceObject : public IndexBufferDeviceObject
{
	GLuint id = 0;
};

/**************************************************
 * Sauce enum to OpenGL enum conversion functions *
 **************************************************/

GLenum toOpenGLDatatype(const Datatype datatype)
{
	switch (datatype)
	{
		case Datatype::Float:   return GL_FLOAT;
		case Datatype::Uint32:  return GL_UNSIGNED_INT;
		case Datatype::Int32:   return GL_INT;
		case Datatype::Uint16:  return GL_UNSIGNED_SHORT;
		case Datatype::Int16:   return GL_SHORT;
		case Datatype::Uint8:   return GL_UNSIGNED_BYTE;
		case Datatype::Int8:    return GL_BYTE;
		case Datatype::Matrix4: return GL_FLOAT_MAT4;
	}
	return GL_FLOAT;
}

GLenum toPrimitiveType(const PrimitiveType primitiveType)
{
	switch (primitiveType)
	{
		case PrimitiveType::PRIMITIVE_POINTS:         return GL_POINTS;
		case PrimitiveType::PRIMITIVE_LINES:          return GL_LINES;
		case PrimitiveType::PRIMITIVE_LINE_STRIP:     return GL_LINE_STRIP;
		case PrimitiveType::PRIMITIVE_LINE_LOOP:      return GL_LINE_LOOP;
		case PrimitiveType::PRIMITIVE_TRIANGLES:      return GL_TRIANGLES;
		case PrimitiveType::PRIMITIVE_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
		case PrimitiveType::PRIMITIVE_TRIANGLE_FAN:   return GL_TRIANGLE_FAN;
	}
	return GL_TRIANGLES;
}

GLenum toBlendFactor(const BlendFactor blendFactor)
{
	switch (blendFactor)
	{
		case BlendFactor::BLEND_ZERO: return GL_ZERO;
		case BlendFactor::BLEND_ONE: return GL_ONE;
		case BlendFactor::BLEND_SRC_COLOR: return GL_SRC_COLOR;
		case BlendFactor::BLEND_ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::BLEND_SRC_ALPHA: return GL_SRC_ALPHA;
		case BlendFactor::BLEND_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::BLEND_DST_ALPHA: return GL_DST_ALPHA;
		case BlendFactor::BLEND_ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::BLEND_DST_COLOR: return GL_DST_COLOR;
		case BlendFactor::BLEND_ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::BLEND_SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
	}
	return GL_ZERO;
}

GLenum toBufferUsage(const BufferUsage bufferUsage)
{
	switch (bufferUsage)
	{
		case BufferUsage::Static:  return GL_STATIC_DRAW;
		case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
		case BufferUsage::Stream:  return GL_STREAM_DRAW;
	}
	return GL_STATIC_DRAW;
}

GLint toPixelInternalFormat(const PixelComponents& fmt, const PixelDatatype& dt)
{
	switch (fmt)
	{
		case PixelComponents::R:
		{
			switch (dt)
			{
				case PixelDatatype::INT: return GL_R32I;
				case PixelDatatype::UNSIGNED_INT: return GL_R32UI;
				case PixelDatatype::BYTE: return GL_R8_SNORM;
				case PixelDatatype::UNSIGNED_BYTE: return GL_R8;
				case PixelDatatype::FLOAT: return GL_R32F;
			}
		}
		break;
		case PixelComponents::RG:
		{
			switch (dt)
			{
				case PixelDatatype::INT: return GL_RG32I;
				case PixelDatatype::UNSIGNED_INT: return GL_RG32UI;
				case PixelDatatype::BYTE: return GL_RG8_SNORM;
				case PixelDatatype::UNSIGNED_BYTE: return GL_RG8;
				case PixelDatatype::FLOAT: return GL_RG32F;
			}
		}
		break;
		case PixelComponents::RGB:
		{
			switch (dt)
			{
				case PixelDatatype::INT: return GL_RGB32I;
				case PixelDatatype::UNSIGNED_INT: return GL_RGB32UI;
				case PixelDatatype::BYTE: return GL_RGB8_SNORM;
				case PixelDatatype::UNSIGNED_BYTE: return GL_RGB8;
				case PixelDatatype::FLOAT: return GL_RGB32F;
			}
		}
		break;
		case PixelComponents::RGBA:
		{
			switch (dt)
			{
				case PixelDatatype::INT: return GL_RGBA32I;
				case PixelDatatype::UNSIGNED_INT: return GL_RGBA32UI;
				case PixelDatatype::BYTE: return GL_RGBA8_SNORM;
				case PixelDatatype::UNSIGNED_BYTE: return GL_RGBA8;
				case PixelDatatype::FLOAT: return GL_RGBA32F;
			}
		}
		break;
	}
	return 0;
}

GLenum toPixelFormat(const PixelComponents& fmt, const PixelDatatype& dt)
{
	switch (fmt)
	{
		case PixelComponents::R:
		{
			switch (dt)
			{
				case PixelDatatype::UNSIGNED_BYTE: case PixelDatatype::BYTE: case PixelDatatype::FLOAT: return GL_RED;
				case PixelDatatype::UNSIGNED_INT: case PixelDatatype::INT: return GL_RED_INTEGER;
			}
		}
		break;
		case PixelComponents::RG:
		{
			switch (dt)
			{
				case PixelDatatype::UNSIGNED_BYTE: case PixelDatatype::BYTE: case PixelDatatype::FLOAT: return GL_RG;
				case PixelDatatype::UNSIGNED_INT: case PixelDatatype::INT: return GL_RG_INTEGER;
			}
		}
		break;
		case PixelComponents::RGB:
		{
			switch (dt)
			{
				case PixelDatatype::UNSIGNED_BYTE: case PixelDatatype::BYTE: case PixelDatatype::FLOAT: return GL_RGB;
				case PixelDatatype::UNSIGNED_INT: case PixelDatatype::INT: return GL_RGB_INTEGER;
			}
		}
		break;
		case PixelComponents::RGBA:
		{
			switch (dt)
			{
				case PixelDatatype::UNSIGNED_BYTE: case PixelDatatype::BYTE: case PixelDatatype::FLOAT: return GL_RGBA;
				case PixelDatatype::UNSIGNED_INT: case PixelDatatype::INT: return GL_RGBA_INTEGER;
			}
		}
		break;
	}
	return 0;
}

GLenum toPixelDatatype(const PixelDatatype& dt)
{
	switch (dt)
	{
		case PixelDatatype::INT: return GL_INT;
		case PixelDatatype::UNSIGNED_INT: return GL_UNSIGNED_INT;
		case PixelDatatype::BYTE: return GL_BYTE;
		case PixelDatatype::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
		case PixelDatatype::FLOAT: return GL_FLOAT;
	}
	return 0;
}

GLint toTextureMagFilter(const TextureFiltering filtering)
{
	switch (filtering)
	{
		case TextureFiltering::NEAREST: return GL_NEAREST;
		case TextureFiltering::LINEAR: return GL_LINEAR;
	}
	return GL_NEAREST;
}

GLint toTextureMinFilter(const TextureFiltering filtering, const bool hasMipmaps)
{
	switch (filtering)
	{
		case TextureFiltering::NEAREST: return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
		case TextureFiltering::LINEAR: return hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
	}
	return hasMipmaps ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
}

GLint toTextureWrapping(const TextureWrapping wrapping)
{
	switch (wrapping)
	{
		case TextureWrapping::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
		case TextureWrapping::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
		case TextureWrapping::REPEAT: return GL_REPEAT;
		case TextureWrapping::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
	}
	return GL_CLAMP_TO_BORDER;
}

/**************************************************
 * OpenGL error handling functions                *
 **************************************************/

#ifdef SAUCE_DEBUG
void checkOpenGLError(const string& info)
{
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR)
	{
		string errorType = "";
		switch (error)
		{
			case GL_INVALID_ENUM: errorType = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE: errorType = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: errorType = "GL_INVALID_OPERATION"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: errorType = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			case GL_OUT_OF_MEMORY: errorType = "GL_OUT_OF_MEMORY"; break;
			case GL_STACK_UNDERFLOW: errorType = "GL_STACK_UNDERFLOW"; break;
			case GL_STACK_OVERFLOW: errorType = "GL_STACK_OVERFLOW"; break;
			default: errorType = "Unknown GL error"; break;
		}
		THROW("glGetError() returned 0x%X (%s) from function %s", error, errorType.c_str(), info.c_str());
	}
}
#endif

#ifdef SAUCE_DEBUG
extern void checkOpenGLError(const string& info);
#define GL_CALL(call) call; checkOpenGLError(#call)
#else
#define GL_CALL(call) call
#endif

/**************************************************
 * OpenGLContext implementation                   *
 **************************************************/

OpenGLContext::OpenGLContext(const int major, const int minor) :
	m_majorVersion(major),
	m_minorVersion(minor)
{
}

OpenGLContext::~OpenGLContext()
{
	GL_CALL(glDeleteBuffers(1, &g_vbo));
	GL_CALL(glDeleteVertexArrays(1, &g_vao));
	delete g_zeroedTextureDataArray;
	SDL_GL_DeleteContext(m_context);
}

void OpenGLContext::enable(const Capability cap)
{
	switch(cap)
	{
		case Capability::BLEND:          GL_CALL(glEnable(GL_BLEND)); break;
		case Capability::DEPTH_TEST:     GL_CALL(glEnable(GL_DEPTH_TEST)); break;
		case Capability::FACE_CULLING:   GL_CALL(glEnable(GL_CULL_FACE)); break;
		case Capability::LINE_SMOOTH:    GL_CALL(glEnable(GL_LINE_SMOOTH)); break;
		case Capability::POLYGON_SMOOTH: GL_CALL(glEnable(GL_POLYGON_SMOOTH)); break;
		case Capability::MULTISAMPLE:    GL_CALL(glEnable(GL_MULTISAMPLE)); break;
		case Capability::TEXTURE_1D:     GL_CALL(glEnable(GL_TEXTURE_1D)); break;
		case Capability::TEXTURE_2D:     GL_CALL(glEnable(GL_TEXTURE_2D)); break;
		case Capability::TEXTURE_3D:     GL_CALL(glEnable(GL_TEXTURE_3D)); break;
		case Capability::VSYNC:
			SDL_GL_SetSwapInterval(1);
			break;
		case Capability::WIREFRAME:
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			break;
	}
}

void OpenGLContext::disable(const Capability cap)
{
	switch(cap)
	{
		case Capability::BLEND:          GL_CALL(glDisable(GL_BLEND)); break;
		case Capability::DEPTH_TEST:     GL_CALL(glDisable(GL_DEPTH_TEST)); break;
		case Capability::FACE_CULLING:   GL_CALL(glDisable(GL_CULL_FACE)); break;
		case Capability::LINE_SMOOTH:    GL_CALL(glDisable(GL_LINE_SMOOTH)); break;
		case Capability::POLYGON_SMOOTH: GL_CALL(glDisable(GL_POLYGON_SMOOTH)); break;
		case Capability::MULTISAMPLE:    GL_CALL(glDisable(GL_MULTISAMPLE)); break;
		case Capability::TEXTURE_1D:     GL_CALL(glDisable(GL_TEXTURE_1D)); break;
		case Capability::TEXTURE_2D:     GL_CALL(glDisable(GL_TEXTURE_2D)); break;
		case Capability::TEXTURE_3D:     GL_CALL(glDisable(GL_TEXTURE_3D)); break;
		case Capability::VSYNC:
			SDL_GL_SetSwapInterval(0);
			break;
		case Capability::WIREFRAME:
			GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
			break;
	}
}

bool OpenGLContext::isEnabled(const Capability cap)
{
	const bool enabled = (bool)GL_CALL(glIsEnabled((uint32)cap));
	return enabled;
}

void OpenGLContext::setPointSize(const float pointSize)
{
	GL_CALL(glPointSize(pointSize));
}

void OpenGLContext::setLineWidth(const float lineWidth)
{
	GL_CALL(glLineWidth(lineWidth));
}

void OpenGLContext::clear(const uint32 clearMask, const Color& clearColor, const double clearDepth, const int32 clearStencil)
{
	uint32 glClearMask = 0x0;
	if (clearMask & BufferMask::COLOR_BUFFER)
	{
		GL_CALL(glClearColor(clearColor.getR() / 255.0f, clearColor.getG() / 255.0f, clearColor.getB() / 255.0f, clearColor.getA() / 255.0f));
		glClearMask |= GL_COLOR_BUFFER_BIT;
	}

	if (clearMask & BufferMask::DEPTH_BUFFER)
	{
		GL_CALL(glClearDepth(clearDepth));
		glClearMask |= GL_DEPTH_BUFFER_BIT;
	}

	if (clearMask & BufferMask::STENCIL_BUFFER)
	{
		GL_CALL(glClearStencil(clearStencil));
		glClearMask |= GL_STENCIL_BUFFER_BIT;
	}
	
	GL_CALL(glClear(glClearMask));
}

void OpenGLContext::enableScissor(const int x, const int y, const int w, const int h)
{
	GL_CALL(glEnable(GL_SCISSOR_TEST));
	GL_CALL(glScissor(x, y, w, h));
}

void OpenGLContext::disableScissor()
{
	GL_CALL(glDisable(GL_SCISSOR_TEST));
}

void OpenGLContext::saveScreenshot(string path)
{
	// Get frame buffer data
	uchar *data = new uchar[m_currentState->width * m_currentState->height * 4];
	GL_CALL(glReadBuffer(GL_FRONT));
	GL_CALL(glReadPixels(0, 0, m_currentState->width, m_currentState->height, GL_RGBA, GL_UNSIGNED_BYTE, data));
	GL_CALL(glReadBuffer(GL_BACK));

	// NOTE: This function is not tested!
	Pixmap pixmap(m_currentState->width, m_currentState->height, PixelFormat(PixelComponents::RGBA, PixelDatatype::BYTE), data);
	pixmap.flipY();
	pixmap.saveToFile(path);

	delete[] data;
}

// Orthographic projection
void OpenGLContext::setViewportSize(const uint w, const uint h)
{
	// Set viewport
	GL_CALL(glViewport(0, 0, w, h));
}

Matrix4 OpenGLContext::createOrtographicMatrix(const float l, const float r, const float t, const float b, const float n, const float f) const
{
	/*
	// TODO: Add flipY
	float l = 0.0f,
		r = (float) m_currentState->width,
		b = flipY ? 0.0f : (float) m_currentState->height,
		t = flipY ? (float) m_currentState->height : 0.0f,
		n = -1.0f,
		f = 1.0f;*/

	// Returns an ortographic projection matrix (typically for 2D rendering)
	Matrix4 mat(
		2.0f / (r - l), 0.0f,            0.0f,           -((r + l) / (r - l)),
		0.0f,           2.0f / (t - b),  0.0f,           -((t + b) / (t - b)),
		0.0f,           0.0f,           -2.0f / (f - n), -((f + n) / (f - n)),
		0.0f,           0.0f,            0.0f,            1.0f);
	return mat;
}

Matrix4 OpenGLContext::createPerspectiveMatrix(const float fov, const float aspectRatio, const float zNear, const float zFar) const
{
	// Returns a perspective matrix
	const float s = tanf(math::degToRad(fov / 2.0f));
	Matrix4 mat(
		1.0f / (s * aspectRatio),    0.0f,      0.0f,                             0.0f,
		0.0f,                        1.0f / s,  0.0f,                             0.0f,
		0.0f,                        0.0f,     -(zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear) ,
		0.0f,                        0.0f,     -1.0f,                             0.0f);
	return mat;
}

Matrix4 OpenGLContext::createLookAtMatrix(const Vector3F &position, const Vector3F &fwd) const
{
	const Vector3F worldUp(0.0f, 1.0f, 0.0f);
	const Vector3F right = math::normalize(math::cross(worldUp, fwd));
	const Vector3F up = math::cross(fwd, right);
	Matrix4 cameraMatrix(
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		fwd.x, fwd.y, fwd.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	Matrix4 cameraTranslate(
		1, 0, 0, -position.x,
		0, 1, 0, -position.y,
		0, 0, 1, -position.z,
		0.0f, 0.0f, 0.0f, 1.0f);
	return cameraMatrix * cameraTranslate;
}

Window *OpenGLContext::createWindow(const string &title, const int x, const int y, const int w, const int h, const Uint32 flags)
{
	// Request opengl 3.1 context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_minorVersion);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	m_window = new Window(this, title, x, y, w, h, SDL_WINDOW_OPENGL | flags);

	// Create GL context
	m_context = SDL_GL_CreateContext(m_window->getSDLHandle());

#ifdef SAUCE_COMPILE_WINDOWS
	// Initialize GL3W
	if(gl3wInit() != 0)
	{
		THROW("GL3W did not initialize!");
	}
#endif

	// Print GPU info
	{
		const uchar* vendorString = GL_CALL(glGetString(GL_VENDOR));
		const uchar* versionString = GL_CALL(glGetString(GL_VERSION));
		LOG("** Using GPU: %s (OpenGL %s) **", vendorString, versionString);
	}

#ifdef SAUCE_COMPILE_WINDOWS
	// Check OpenGL support
	if(!gl3wIsSupported(m_majorVersion, m_minorVersion))
	{
		THROW("OpenGL %i.%i not supported\n", m_majorVersion, m_minorVersion);
	}
#endif

	// Setup graphics context
	Vector2I size;
	m_window->getSize(&size.x, &size.y);
	setSize(size.x, size.y);

	// We default to an ortographic projection where top-left is (0, 0) and bottom-right is (w, h)
	setProjectionMatrix(createOrtographicMatrix(0, size.x, 0, size.y));

	// Init graphics
	GL_CALL(glGenVertexArrays(1, &g_vao));
	GL_CALL(glBindVertexArray(g_vao));
	GL_CALL(glGenBuffers(1, &g_vbo));
	GL_CALL(glGenBuffers(1, &g_ibo));

	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

	// Enable blending
	GL_CALL(glEnable(GL_BLEND));
	GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Set smooth lines
	GL_CALL(glEnable(GL_LINE_SMOOTH));
	GL_CALL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	// Set some gl state variables
	GL_CALL(glDepthFunc(GL_LESS));
	GL_CALL(glCullFace(GL_BACK));
	GL_CALL(glPointSize(4));
	GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &g_maxTextureSize));
	g_zeroedTextureDataArray = new GLubyte[g_maxTextureSize * g_maxTextureSize * 4];
	memset(g_zeroedTextureDataArray, 0, g_maxTextureSize * g_maxTextureSize * 4);

	// Create passthrough shader
	{
		const string vertexShader =
			"\n"
			"in vec2 in_Position;\n"
			"in vec2 in_TexCoord;\n"
			"in vec4 in_VertexColor;\n"
			"\n"
			"out vec2 v_TexCoord;\n"
			"out vec4 v_VertexColor;\n"
			"\n"
			"uniform mat4 u_ModelViewProj;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4(in_Position, 0.0, 1.0) * u_ModelViewProj;\n"
			"	v_TexCoord = in_TexCoord;\n"
			"	v_VertexColor = in_VertexColor;\n"
			"}\n";

		const string pixelShader =
			"\n"
			"in vec2 v_TexCoord;\n"
			"in vec4 v_VertexColor;\n"
			"\n"
			"out vec4 out_FragColor;\n"
			"\n"
			"uniform sampler2D u_Texture;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	out_FragColor = texture(u_Texture, v_TexCoord) * v_VertexColor;\n"
			"}\n";

		s_glslVersion = getGLSLVersion();

		ShaderDesc shaderDesc;
		shaderDesc.shaderSourceVS = vertexShader;
		shaderDesc.shaderSourcePS = pixelShader;
		s_defaultShader = CreateNew<Shader>(shaderDesc);
	}

	// Create blank texture
	{
		uint8 pixel[4];
		pixel[0] = pixel[1] = pixel[2] = pixel[3] = 255;
		Pixmap pixmap(1, 1, PixelFormat(PixelComponents::RGBA, PixelDatatype::UNSIGNED_BYTE), pixel);

		Texture2DDesc textureDesc;
		textureDesc.debugName = "DefaultTexture";
		textureDesc.pixmap = &pixmap;
		s_defaultTexture = CreateNew<Texture2D>(textureDesc);
	}

	return m_window;
}

void OpenGLContext::setupContext()
{
	// Set blend func
	GL_CALL(glBlendFuncSeparate(
		toBlendFactor(m_currentState->blendState.m_src),
		toBlendFactor(m_currentState->blendState.m_dst),
		toBlendFactor(m_currentState->blendState.m_alphaSrc),
		toBlendFactor(m_currentState->blendState.m_alphaDst))
	);

	ShaderRef shader = m_currentState->shader;
	if(!shader)
	{
		shader = s_defaultShader;
	}

	if (m_currentState->texture)
	{
		shader->setSampler2D("u_Texture", m_currentState->texture);
	}
	else if (!m_currentState->shader)
	{
		shader->setSampler2D("u_Texture", s_defaultTexture);
	}

	{
		// TODO: Check if all samplers are bound on the shader
	}

	ShaderDeviceObject* shaderDeviceObjectBase;
	shader_getDeviceObject(shader, shaderDeviceObjectBase);
	OpenGLShaderDeviceObject* shaderDeviceObject = dynamic_cast<OpenGLShaderDeviceObject*>(shaderDeviceObjectBase);
	assert(shaderDeviceObject);

	// Enable shader
	GL_CALL(glUseProgram(shaderDeviceObject->id));

	// Set projection matrix
	Matrix4 modelViewProjection = m_currentState->projectionMatrix * m_currentState->transformationMatrixStack.top();
	shader->setUniformMatrix4f("u_ModelViewProj", modelViewProjection.get());

	GLuint target = 0;

	// Set all uniforms
	for(unordered_map<string, ShaderUniform*>::iterator itr = shaderDeviceObject->uniforms.begin(); itr != shaderDeviceObject->uniforms.end(); ++itr)
	{
		const ShaderUniform *uniform = itr->second;
		switch(uniform->type)
		{
			case GL_INT: case GL_BOOL: GL_CALL(glUniform1iv(uniform->loc, uniform->count, (const GLint*) uniform->data)); break;
			case GL_INT_VEC2: case GL_BOOL_VEC2: GL_CALL(glUniform2i(uniform->loc, ((GLint*) uniform->data)[0], ((GLint*) uniform->data)[1])); break;
			case GL_INT_VEC3: case GL_BOOL_VEC3: GL_CALL(glUniform3i(uniform->loc, ((GLint*) uniform->data)[0], ((GLint*) uniform->data)[1], ((GLint*) uniform->data)[2])); break;
			case GL_INT_VEC4: case GL_BOOL_VEC4: GL_CALL(glUniform4i(uniform->loc, ((GLint*) uniform->data)[0], ((GLint*) uniform->data)[1], ((GLint*) uniform->data)[2], ((GLint*) uniform->data)[3])); break;

			case GL_UNSIGNED_INT: GL_CALL(glUniform1ui(uniform->loc, ((GLuint*) uniform->data)[0])); break;
			case GL_UNSIGNED_INT_VEC2: GL_CALL(glUniform2ui(uniform->loc, ((GLuint*) uniform->data)[0], ((GLuint*) uniform->data)[1])); break;
			case GL_UNSIGNED_INT_VEC3: GL_CALL(glUniform3ui(uniform->loc, ((GLuint*) uniform->data)[0], ((GLuint*) uniform->data)[1], ((GLuint*) uniform->data)[2])); break;
			case GL_UNSIGNED_INT_VEC4: GL_CALL(glUniform4ui(uniform->loc, ((GLuint*) uniform->data)[0], ((GLuint*) uniform->data)[1], ((GLuint*) uniform->data)[2], ((GLuint*) uniform->data)[3])); break;

			case GL_FLOAT: GL_CALL(glUniform1f(uniform->loc, ((GLfloat*) uniform->data)[0])); break;
			case GL_FLOAT_VEC2: GL_CALL(glUniform2fv(uniform->loc, uniform->count, (const GLfloat*) uniform->data)); break;
			case GL_FLOAT_VEC3: GL_CALL(glUniform3f(uniform->loc, ((GLfloat*) uniform->data)[0], ((GLfloat*) uniform->data)[1], ((GLfloat*) uniform->data)[2])); break;
			case GL_FLOAT_VEC4: GL_CALL(glUniform4fv(uniform->loc, uniform->count, (const GLfloat*) uniform->data)); break;

			case GL_FLOAT_MAT4: GL_CALL(glUniformMatrix4fv(uniform->loc, 1, GL_FALSE, (GLfloat*) uniform->data)); break;

			case GL_UNSIGNED_INT_SAMPLER_2D:
			case GL_INT_SAMPLER_2D:
			case GL_SAMPLER_2D:
			{
				Texture2DDeviceObject* textureDeviceObjectBase;
				texture2D_getDeviceObject(uniform->texture, textureDeviceObjectBase);
				OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
				assert(textureDeviceObject);

				GL_CALL(glActiveTexture(GL_TEXTURE0 + target));
				GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
				GL_CALL(glUniform1i(uniform->loc, target++));
			}
			break;
		}
	}
}

void OpenGLContext::setupVertexAttributePointers(const VertexFormat& fmt)
{
	const int32 vertexSizeInBytes = fmt.getVertexSizeInBytes();
	for (uint32 i = 0; i < (uint32)VertexAttribute::VERTEX_ATTRIB_MAX; i++)
	{
		VertexAttribute attrib = VertexAttribute(i);
		const uint64 attribOffset = fmt.getAttributeOffset(attrib);
		switch (attrib)
		{
			case VertexAttribute::VERTEX_POSITION:
			{
				if (fmt.isAttributeEnabled(attrib))
				{
					GL_CALL(glEnableVertexAttribArray(0));
					GL_CALL(glVertexAttribPointer(0, fmt.getElementCount(attrib), toOpenGLDatatype(fmt.getDatatype(attrib)), GL_TRUE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
				}
				else
				{
					GL_CALL(glDisableVertexAttribArray(0));
				}
			}
			break;

			case VertexAttribute::VERTEX_COLOR:
			{
				if (fmt.isAttributeEnabled(attrib))
				{
					GL_CALL(glEnableVertexAttribArray(1));
					GL_CALL(glVertexAttribPointer(1, fmt.getElementCount(attrib), toOpenGLDatatype(fmt.getDatatype(attrib)), GL_TRUE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
				}
				else
				{
					GL_CALL(glDisableVertexAttribArray(1));
				}
			}
			break;

			case VertexAttribute::VERTEX_TEX_COORD:
			{
				if (fmt.isAttributeEnabled(attrib))
				{
					GL_CALL(glEnableVertexAttribArray(2));
					GL_CALL(glVertexAttribPointer(2, fmt.getElementCount(attrib), toOpenGLDatatype(fmt.getDatatype(attrib)), GL_FALSE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
				}
				else
				{
					GL_CALL(glDisableVertexAttribArray(2));
				}
			}
			break;

			case VertexAttribute::VERTEX_NORMAL:
			{
				if (fmt.isAttributeEnabled(attrib))
				{
					GL_CALL(glEnableVertexAttribArray(3));
					GL_CALL(glVertexAttribPointer(3, fmt.getElementCount(attrib), toOpenGLDatatype(fmt.getDatatype(attrib)), GL_FALSE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
				}
				else
				{
					GL_CALL(glDisableVertexAttribArray(3));
				}
			}
			break;
		}
	}
}

void OpenGLContext::drawIndexedPrimitives(const PrimitiveType primitiveType, const Vertex *vertices, const uint vertexCount, const uint *indices, const uint indexCount)
{
	// If there are no vertices to draw, do nothing
	if(vertexCount == 0 || indexCount == 0) return;

	setupContext();

	// Get vertices and vertex data
	VertexFormat fmt = vertices->getFormat();
	const int32 vertexSizeInBytes = fmt.getVertexSizeInBytes();
	char *vertexData = new char[vertexCount * vertexSizeInBytes];
	for(uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData(vertexData + i * vertexSizeInBytes);
	}

	// Bind buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, g_vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSizeInBytes, vertexData, GL_DYNAMIC_DRAW));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indices, GL_DYNAMIC_DRAW));

	// Setup vertex attribute pointers
	setupVertexAttributePointers(fmt);

	// Draw primitives
	GL_CALL(glDrawElements(toPrimitiveType(primitiveType), indexCount, GL_UNSIGNED_INT, 0));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	// Release vertex data
	delete[] vertexData;
}

void OpenGLContext::drawIndexedPrimitives(const PrimitiveType primitiveType, const VertexBufferRef vertexBuffer, const IndexBufferRef indexBuffer)
{
	// If one of the buffers are empty, do nothing
	if (vertexBuffer->getVertexCount() == 0 || indexBuffer->getIndexCount() == 0)
	{
		return;
	}

	setupContext();

	// Bind vertex buffer object
	VertexFormat vertexFormat;
	{
		VertexBufferDeviceObject* vertexBufferDeviceObject;
		vertexBuffer_getDeviceObject(vertexBuffer, vertexBufferDeviceObject);
		vertexBuffer_bindVertexBuffer(vertexBufferDeviceObject);
		vertexFormat = vertexBufferDeviceObject->vertexFormat;
	}

	// Bind index buffer object
	uint32 indexCount;
	{
		IndexBufferDeviceObject* indexBufferDeviceObject;
		indexBuffer_getDeviceObject(indexBuffer, indexBufferDeviceObject);
		indexBuffer_bindIndexBuffer(indexBufferDeviceObject);
		indexCount = indexBufferDeviceObject->indexCount;
	}

	// Setup vertex attribute pointers
	setupVertexAttributePointers(vertexFormat);

	// Draw vbo
	GL_CALL(glDrawElements(toPrimitiveType(primitiveType), indexCount, GL_UNSIGNED_INT, 0));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void OpenGLContext::drawPrimitives(const PrimitiveType primitiveType, const Vertex *vertices, const uint vertexCount)
{
	// If there are no vertices to draw, do nothing
	if(vertexCount == 0) return;

	setupContext();

	// Get vertices and vertex data
	const VertexFormat fmt = vertices->getFormat();
	const int32 vertexSizeInBytes = fmt.getVertexSizeInBytes();
	char *vertexData = new char[vertexCount * vertexSizeInBytes];
	for(uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData(vertexData + i * vertexSizeInBytes);
	}

	// Bind buffer
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, g_vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSizeInBytes, vertexData, GL_DYNAMIC_DRAW));

	// Setup vertex attribute pointers
	setupVertexAttributePointers(fmt);

	// Draw primitives
	GL_CALL(glDrawArrays(toPrimitiveType(primitiveType), 0, vertexCount));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// Release vertex data
	delete[] vertexData;
}

void OpenGLContext::drawPrimitives(const PrimitiveType primitiveType, const VertexBufferRef vertexBuffer)
{
	// If the buffer is empty, do nothing
	if (vertexBuffer->getVertexCount() == 0)
	{
		return;
	}

	setupContext();

	// Bind vertex buffer object
	VertexFormat vertexFormat;
	uint32 vertexCount;
	{
		VertexBufferDeviceObject* vertexBufferDeviceObject;
		vertexBuffer_getDeviceObject(vertexBuffer, vertexBufferDeviceObject);
		vertexBuffer_bindVertexBuffer(vertexBufferDeviceObject);
		vertexFormat = vertexBufferDeviceObject->vertexFormat;
		vertexCount = vertexBufferDeviceObject->vertexCount;
	}

	// Setup vertex attribute pointers
	setupVertexAttributePointers(vertexFormat);

	// Draw vbo
	GL_CALL(glDrawArrays(toPrimitiveType(primitiveType), 0, vertexCount));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

string OpenGLContext::getGLSLVersion() const
{
	switch(m_majorVersion)
	{
		case 2:
		{
			switch(m_minorVersion)
			{
				case 0: return "110";
				case 1: return "120";
			}
		}

		case 3:
		{
			switch(m_minorVersion)
			{
				case 0: return "130";
				case 1: return "140";
				case 2: return "150";
				case 3: return "330";
			}
		}

		case 4:
		{
			switch(m_minorVersion)
			{
				case 0: return "400";
				case 1: return "410";
				case 2: return "420";
				case 3: return "430";
				case 4: return "440";
				case 5: return "450";
				case 6: return "460";
			}
		}
	}
	return "150";
}

/**************************************************
 * Texture2D internal API implementation          *
 **************************************************/

void OpenGLContext::texture2D_createDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject, const string& deviceObjectName)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = new OpenGLTexture2DDeviceObject();

	// Create OpenGL-side texture object
	GLuint textureID;
	GL_CALL(glGenTextures(1, &textureID));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureID));
	GL_CALL(glObjectLabel(GL_TEXTURE, textureID, deviceObjectName.size(), deviceObjectName.c_str()));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	// Update device object settings
	textureDeviceObject->id = textureID;

	outTextureDeviceObject = textureDeviceObject;
}

void OpenGLContext::texture2D_destroyDeviceObject(Texture2DDeviceObject*& outTextureDeviceObject)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(outTextureDeviceObject);
	assert(textureDeviceObject);
	GL_CALL(glDeleteTextures(1, &textureDeviceObject->id));
	delete textureDeviceObject;
	outTextureDeviceObject = nullptr;
}

void OpenGLContext::texture2D_copyToGPU(Texture2DDeviceObject* textureDeviceObjectBase, const PixelFormat pixelFormat, const uint32 width, const uint32 height, uint8* textureData)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	const GLint  internalFormat = toPixelInternalFormat(pixelFormat.getComponents(), pixelFormat.getDataType());
	const GLenum format         = toPixelFormat(pixelFormat.getComponents(), pixelFormat.getDataType());
	const GLenum datatype       = toPixelDatatype(pixelFormat.getDataType());

	// Upload texture data to GPU
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glTexImage2D(
		GL_TEXTURE_2D,
		0,
		internalFormat,
		(GLsizei)width,
		(GLsizei)height,
		0,
		format,
		datatype,
		(const GLvoid*)textureData)
	);
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	// Update device object settings
	textureDeviceObject->width = width;
	textureDeviceObject->height = height;
	textureDeviceObject->pixelFormat = pixelFormat;
}

void OpenGLContext::texture2D_copyToCPUReadable(Texture2DDeviceObject* textureDeviceObjectBase, uint8** outTextureData)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	const PixelFormat& pixelFormat = textureDeviceObject->pixelFormat;
	const GLenum format   = toPixelFormat(pixelFormat.getComponents(), pixelFormat.getDataType());
	const GLenum datatype = toPixelDatatype(pixelFormat.getDataType());

	// Copy texture data to CPU-readable memory
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glGetTexImage(
		GL_TEXTURE_2D,
		0,
		format,
		datatype,
		(GLvoid*)*outTextureData)
	);
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void OpenGLContext::texture2D_updateSubregion(Texture2DDeviceObject* textureDeviceObjectBase, const uint32 x, const uint32 y, const uint32 subRegionWidth, const uint32 subRegionHeight, uint8* textureData)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	const PixelFormat& pixelFormat = textureDeviceObject->pixelFormat;
	const GLenum format   = toPixelFormat(pixelFormat.getComponents(), pixelFormat.getDataType());
	const GLenum datatype = toPixelDatatype(pixelFormat.getDataType());

	// Update texture subregion
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D,
		0,
		(GLint)x,
		(GLint)y,
		(GLsizei)subRegionWidth,
		(GLsizei)subRegionHeight,
		format,
		datatype,
		(const GLvoid*)textureData)
	);
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void OpenGLContext::texture2D_updateFiltering(Texture2DDeviceObject* textureDeviceObjectBase, const TextureFiltering filtering)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	// Update texture filtering
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toTextureMinFilter(filtering, textureDeviceObject->hasMipmaps)));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toTextureMagFilter(filtering)));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	// Update device object settings
	textureDeviceObject->filtering = filtering;
}

void OpenGLContext::texture2D_updateWrapping(Texture2DDeviceObject* textureDeviceObjectBase, const TextureWrapping wrapping)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	// Update texture wrapping
	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toTextureWrapping(wrapping)));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toTextureWrapping(wrapping)));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	// Update device object settings
	textureDeviceObject->wrapping = wrapping;
}

void OpenGLContext::texture2D_clearTexture(Texture2DDeviceObject* textureDeviceObjectBase)
{
	OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
	assert(textureDeviceObject);

	GL_CALL(glBindTexture(GL_TEXTURE_2D, textureDeviceObject->id));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureDeviceObject->width, textureDeviceObject->height, GL_BGRA, GL_UNSIGNED_BYTE, g_zeroedTextureDataArray));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

/**************************************************
 * Shader internal API implementation             *
 **************************************************/

void OpenGLContext::shader_createDeviceObject(ShaderDeviceObject*& outShaderDeviceObject, const string& deviceObjectName)
{
	OpenGLShaderDeviceObject* shaderDeviceObject = new OpenGLShaderDeviceObject();

	// Create OpenGL-side shader program
	GLuint shaderProgramID = GL_CALL(glCreateProgram());
	GL_CALL(glObjectLabel(GL_PROGRAM, shaderProgramID, deviceObjectName.size(), deviceObjectName.c_str()));

	// Update device object settings
	shaderDeviceObject->id = shaderProgramID;

	outShaderDeviceObject = shaderDeviceObject;
}

void OpenGLContext::shader_destroyDeviceObject(ShaderDeviceObject*& outShaderDeviceObject)
{
	OpenGLShaderDeviceObject* shaderDeviceObject = dynamic_cast<OpenGLShaderDeviceObject*>(outShaderDeviceObject);
	assert(shaderDeviceObject);
	for (const pair<string, ShaderUniform*>& kv : shaderDeviceObject->uniforms)
	{
		delete kv.second;
	}
	delete shaderDeviceObject;
	outShaderDeviceObject = nullptr;
}

void OpenGLContext::shader_compileShader(ShaderDeviceObject* shaderDeviceObjectBase, const string& vsSource, const string& psSource, const string& gsSource)
{
	OpenGLShaderDeviceObject* shaderDeviceObject = dynamic_cast<OpenGLShaderDeviceObject*>(shaderDeviceObjectBase);
	assert(shaderDeviceObject);

	// Create vertex, pixel and geometry shaders
	bool hasGeometryShader = !gsSource.empty();
	GLuint vertexShaderID = GL_CALL(glCreateShader(GL_VERTEX_SHADER));
	GLuint pixelShaderID = GL_CALL(glCreateShader(GL_FRAGMENT_SHADER));
	GLuint geometryShaderID = 0;
	if (hasGeometryShader)
	{
		geometryShaderID = GL_CALL(glCreateShader(GL_GEOMETRY_SHADER));
	}

	auto prependVersionString = [](string& sourceStr)
	{
		// Removing existing #version
		// This assumes that #version is the first thing that appears in the file,
		// and that #version is followed by 4 characters
		if (sourceStr.size() > 12)
		{
			if (sourceStr.substr(0, 8) == "#version")
			{
				sourceStr = sourceStr.substr(12);
			}
		}

		// Prepend correct version string
		sourceStr = "#version " + s_glslVersion + "\n" + sourceStr;
	};

	// Create modified shader code
	string vsSourceModified = vsSource;
	prependVersionString(vsSourceModified);
	string psSourceModified = psSource;
	prependVersionString(psSourceModified);
	string gsSourceModified = gsSource;
	prependVersionString(gsSourceModified);

	// Result variables
	auto compileShader = [](const string& sourceStr, const GLuint shaderID)
	{
		const char* sourceCode = sourceStr.c_str();
		const int sourceLength = sourceStr.length();
		GL_CALL(glShaderSource(shaderID, 1, &sourceCode, &sourceLength));
		GL_CALL(glCompileShader(shaderID));

		// Validate shader
		GLint success;
		GL_CALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
		if (!success)
		{
			// Get log message and throw as exception
			GLint logLength;
			GL_CALL(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength));
			string compileLog;
			compileLog.resize(logLength);
			GL_CALL(glGetShaderInfoLog(shaderID, logLength, NULL, &compileLog[0]));
			THROW(compileLog.c_str());
		}
	};

	// Compile vertex shader
	{
		LOG("Compiling vertex shader...");
		compileShader(vsSourceModified, vertexShaderID);
	}

	// Compile pixel shader
	{
		LOG("Compiling pixel shader...");
		compileShader(psSourceModified, pixelShaderID);
	}

	// Compile geometry shader
	if (hasGeometryShader)
	{
		LOG("Compiling geometry shader...");
		compileShader(gsSourceModified, geometryShaderID);
	}

	// Create shader program
	GL_CALL(glAttachShader(shaderDeviceObject->id, vertexShaderID));
	GL_CALL(glAttachShader(shaderDeviceObject->id, pixelShaderID));
	if (hasGeometryShader)
	{
		glAttachShader(shaderDeviceObject->id, geometryShaderID);
	}

	GL_CALL(glBindAttribLocation(shaderDeviceObject->id, 0, "in_Position"));
	GL_CALL(glBindAttribLocation(shaderDeviceObject->id, 1, "in_VertexColor"));
	GL_CALL(glBindAttribLocation(shaderDeviceObject->id, 2, "in_TexCoord"));
	GL_CALL(glBindAttribLocation(shaderDeviceObject->id, 3, "in_Normal"));
	GL_CALL(glBindFragDataLocation(shaderDeviceObject->id, 0, "out_FragColor"));
	
	// Link the shader program
	{
		LOG("Linking shader program...");

		// Link program
		GL_CALL(glLinkProgram(shaderDeviceObject->id));

		// Check if link was successful 
		GLint success;
		GL_CALL(glGetProgramiv(shaderDeviceObject->id, GL_LINK_STATUS, &success));
		if (!success)
		{
			// Get log message and throw as exception
			GLint logLength;
			GL_CALL(glGetProgramiv(shaderDeviceObject->id, GL_INFO_LOG_LENGTH, &logLength));
			string compileLog;
			compileLog.resize(logLength);
			GL_CALL(glGetProgramInfoLog(shaderDeviceObject->id, logLength, NULL, &compileLog[0]));
			THROW(compileLog.c_str());
		}
	}

	// Setup uniform mapping
	{
		GLint uniformCount;
		GL_CALL(glGetProgramiv(shaderDeviceObject->id, GL_ACTIVE_UNIFORMS, &uniformCount));
		GLint uniformNameLength, uniformArraySize;
		GLenum uniformType;
		GLchar uniformName[256];
		for (int uniformIndex = 0; uniformIndex < uniformCount; ++uniformIndex)
		{
			GL_CALL(glGetActiveUniform(
				shaderDeviceObject->id,
				uniformIndex,
				256,
				&uniformNameLength,
				&uniformArraySize,
				&uniformType,
				uniformName)
			);

			string uniformNameStr = uniformName;
			
			// Skip gl_ uniforms
			if (uniformNameStr.substr(0, 3) == "gl_")
			{
				continue;
			}

			// Strip trailing "[0]"s
			if (uniformNameStr.length() > 3 && uniformNameStr.substr(uniformNameStr.length() - 3) == "[0]")
			{
				uniformNameStr = uniformNameStr.substr(0, uniformNameStr.length() - 3);
			}

			ShaderUniform* uniform = new ShaderUniform;
			uniform->type = uniformType;
			uniform->loc = GL_CALL(glGetUniformLocation(shaderDeviceObject->id, uniformName));
			uniform->count = uniformArraySize;

			const uint32 INT_SIZE   = sizeof(GLint);
			const uint32 FLOAT_SIZE = sizeof(GLfloat);
			const uint32 PTR_SIZE   = sizeof(void*);

			uint32 elementDataSize = 0;
			switch (uniformType)
			{
				case GL_SAMPLER_2D:
				case GL_INT_SAMPLER_2D:
				case GL_UNSIGNED_INT_SAMPLER_2D: elementDataSize = 0; break;

				case GL_INT: case GL_BOOL:           elementDataSize = INT_SIZE * 1; break;
				case GL_INT_VEC2: case GL_BOOL_VEC2: elementDataSize = INT_SIZE * 2; break;
				case GL_INT_VEC3: case GL_BOOL_VEC3: elementDataSize = INT_SIZE * 3; break;
				case GL_INT_VEC4: case GL_BOOL_VEC4: elementDataSize = INT_SIZE * 4; break;

				case GL_UNSIGNED_INT:      elementDataSize = INT_SIZE * 1; break;
				case GL_UNSIGNED_INT_VEC2: elementDataSize = INT_SIZE * 2; break;
				case GL_UNSIGNED_INT_VEC3: elementDataSize = INT_SIZE * 3; break;
				case GL_UNSIGNED_INT_VEC4: elementDataSize = INT_SIZE * 4; break;

				case GL_FLOAT:      elementDataSize = FLOAT_SIZE * 1; break;
				case GL_FLOAT_VEC2: elementDataSize = FLOAT_SIZE * 2; break;
				case GL_FLOAT_VEC3: elementDataSize = FLOAT_SIZE * 3; break;
				case GL_FLOAT_VEC4: elementDataSize = FLOAT_SIZE * 4; break;
				case GL_FLOAT_MAT4: elementDataSize = FLOAT_SIZE * 16; break;

				default:
					LOG("Unknown uniform type encountered");
					break;
			}

			if (elementDataSize > 0)
			{
				uniform->data = new uint8[elementDataSize * uniformArraySize];
			}

			shaderDeviceObject->uniforms[uniformNameStr] = uniform;
		}
	}
}

void OpenGLContext::shader_setUniform(ShaderDeviceObject* shaderDeviceObjectBase, const string& uniformName, const Datatype datatype, const uint32 numComponentsPerElement, const uint32 numElements, const void* data)
{
	OpenGLShaderDeviceObject* shaderDeviceObject = dynamic_cast<OpenGLShaderDeviceObject*>(shaderDeviceObjectBase);
	assert(shaderDeviceObject);

	unordered_map<string, ShaderUniform*>::iterator itr = shaderDeviceObject->uniforms.find(uniformName);
	if (itr != shaderDeviceObject->uniforms.end())
	{
		ShaderUniform* uniform = itr->second;

		bool doesDatatypeMatch = false;
		int32 datatypeInBytes = util::GetDatatypeSize(datatype);
		switch (datatype)
		{
			case Datatype::Int8:
			case Datatype::Int16:
			case Datatype::Int32:
			{
				switch (numComponentsPerElement)
				{
					case 1: doesDatatypeMatch = uniform->type == GL_INT || uniform->type == GL_BOOL; break;
					case 2: doesDatatypeMatch = uniform->type == GL_INT_VEC2 || uniform->type == GL_BOOL_VEC2; break;
					case 3: doesDatatypeMatch = uniform->type == GL_INT_VEC3 || uniform->type == GL_BOOL_VEC3; break;
					case 4: doesDatatypeMatch = uniform->type == GL_INT_VEC4 || uniform->type == GL_BOOL_VEC4; break;
					default:
						LOG("shader_setUniform(): numComponentsPerElement must be 1-4");
						break;
				}
			}
			break;
			case Datatype::Uint8:
			case Datatype::Uint16:
			case Datatype::Uint32:
			{
				switch (numComponentsPerElement)
				{
					case 1: doesDatatypeMatch = uniform->type == GL_UNSIGNED_INT; break;
					case 2: doesDatatypeMatch = uniform->type == GL_UNSIGNED_INT_VEC2; break;
					case 3: doesDatatypeMatch = uniform->type == GL_UNSIGNED_INT_VEC3; break;
					case 4: doesDatatypeMatch = uniform->type == GL_UNSIGNED_INT_VEC4; break;
					default:
						LOG("shader_setUniform(): numComponentsPerElement must be 1-4");
						break;
				}
			}
			break;
			case Datatype::Float:
			{
				switch (numComponentsPerElement)
				{
					case 1: doesDatatypeMatch = uniform->type == GL_FLOAT; break;
					case 2: doesDatatypeMatch = uniform->type == GL_FLOAT_VEC2; break;
					case 3: doesDatatypeMatch = uniform->type == GL_FLOAT_VEC3; break;
					case 4: doesDatatypeMatch = uniform->type == GL_FLOAT_VEC4; break;
					default:
						LOG("shader_setUniform(): numComponentsPerElement must be 1-4");
						break;
				}
			}
			break;
			case Datatype::Matrix4:
			{
				switch (numComponentsPerElement)
				{
					case 16:
						doesDatatypeMatch = uniform->type == GL_FLOAT_MAT4;
						datatypeInBytes = 4;
						break;
					default:
						LOG("shader_setUniform(): numComponentsPerElement must be 16 for 'Matrix4's");
						break;
				}
			}
			break;
		}

		if (doesDatatypeMatch)
		{
			if (uniform->count == numElements)
			{
				memcpy(uniform->data, data, datatypeInBytes * numComponentsPerElement * numElements);
			}
			else
			{
				LOG("shader_setUniform(): Uniform array '%s' has %i elements, got %i", uniformName.c_str(), uniform->count, numElements);
			}
		}
		else
		{
			LOG("shader_setUniform(): Uniform '%s' datatype mismatch", uniformName.c_str());
		}
	}
	else
	{
		LOG("shader_setUniform(): Uniform '%s' does not exist", uniformName.c_str());
	}
}

void OpenGLContext::shader_setSampler2D(ShaderDeviceObject* shaderDeviceObjectBase, const string& uniformName, Texture2DRef texture)
{
	OpenGLShaderDeviceObject* shaderDeviceObject = dynamic_cast<OpenGLShaderDeviceObject*>(shaderDeviceObjectBase);
	assert(shaderDeviceObject);

	unordered_map<string, ShaderUniform*>::iterator itr = shaderDeviceObject->uniforms.find(uniformName);
	if (itr != shaderDeviceObject->uniforms.end())
	{
		ShaderUniform* uniform = itr->second;

		if (uniform->type == GL_SAMPLER_2D ||
			uniform->type == GL_INT_SAMPLER_2D ||
			uniform->type == GL_UNSIGNED_INT_SAMPLER_2D)
		{
			uniform->texture = texture;
		}
		else
		{
			LOG("Uniform '%s' is not type 'gsampler2D'", uniformName.c_str());
		}
	}
	else
	{
		LOG("Uniform '%s' does not exist.", uniformName.c_str());
	}
}

/**************************************************
 * RenderTarget2D API implementation              *
 **************************************************/

void OpenGLContext::renderTarget2D_createDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject, const string& deviceObjectName)
{
	OpenGLRenderTarget2DDeviceObject* renderTargetDeviceObject = new OpenGLRenderTarget2DDeviceObject();

	// Create OpenGL-side frame buffer object
	GLuint renderTargetID;
	GL_CALL(glGenFramebuffers(1, &renderTargetID));
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, renderTargetID));
	GL_CALL(glObjectLabel(GL_FRAMEBUFFER, renderTargetID, deviceObjectName.size(), deviceObjectName.c_str()));
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// Update device object settings
	renderTargetDeviceObject->id = renderTargetID;

	outRenderTargetDeviceObject = renderTargetDeviceObject;
}

void OpenGLContext::renderTarget2D_destroyDeviceObject(RenderTarget2DDeviceObject*& outRenderTargetDeviceObject)
{
	OpenGLRenderTarget2DDeviceObject* renderTargetDeviceObject = dynamic_cast<OpenGLRenderTarget2DDeviceObject*>(outRenderTargetDeviceObject);
	assert(renderTargetDeviceObject);

	// Delete OpenGL-side frame buffer object
	GL_CALL(glDeleteFramebuffers(1, &renderTargetDeviceObject->id));

	// Free device object
	delete renderTargetDeviceObject->targetTextures;
	delete renderTargetDeviceObject;

	outRenderTargetDeviceObject = nullptr;
}

void OpenGLContext::renderTarget2D_initializeRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObjectBase, const Texture2DRef* targetTextures, const uint32 targetCount)
{
	OpenGLRenderTarget2DDeviceObject* renderTargetDeviceObject = dynamic_cast<OpenGLRenderTarget2DDeviceObject*>(renderTargetDeviceObjectBase);
	assert(renderTargetDeviceObject);

	// Copy target texture references
	assert(targetCount > 0);
	Texture2DRef* targetTexturesCopy = new Texture2DRef[targetCount];
	for (uint32 i = 0; i < targetCount; ++i)
	{
		targetTexturesCopy[i] = targetTextures[i];
	}

	// Update device object settings
	renderTargetDeviceObject->targetCount = targetCount;
	renderTargetDeviceObject->targetTextures = targetTexturesCopy;
}

void OpenGLContext::renderTarget2D_bindRenderTarget(RenderTarget2DDeviceObject* renderTargetDeviceObjectBase)
{
	if (renderTargetDeviceObjectBase)
	{
		OpenGLRenderTarget2DDeviceObject* renderTargetDeviceObject = dynamic_cast<OpenGLRenderTarget2DDeviceObject*>(renderTargetDeviceObjectBase);
		assert(renderTargetDeviceObject);

		static GLenum targetColorAttachments[32];

		// Bind framebuffer
		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, renderTargetDeviceObject->id));
		for (uint32 i = 0; i < renderTargetDeviceObject->targetCount; ++i)
		{
			Texture2DDeviceObject* textureDeviceObjectBase;
			texture2D_getDeviceObject(renderTargetDeviceObject->targetTextures[i], textureDeviceObjectBase);
			OpenGLTexture2DDeviceObject* textureDeviceObject = dynamic_cast<OpenGLTexture2DDeviceObject*>(textureDeviceObjectBase);
			assert(textureDeviceObject);

			GLenum targetColorAttachment = GL_COLOR_ATTACHMENT0 + i;
			GL_CALL(glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				targetColorAttachment,
				GL_TEXTURE_2D,
				textureDeviceObject->id,
				0)
			);
			targetColorAttachments[i] = targetColorAttachment;
		}
		GL_CALL(glDrawBuffers(renderTargetDeviceObject->targetCount, targetColorAttachments));
	}
	else
	{
		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}

/**************************************************
 * VertexBuffer API implementation                *
 **************************************************/

void OpenGLContext::vertexBuffer_createDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject, const string& deviceObjectName)
{
	OpenGLVertexBufferDeviceObject* vertexBufferDeviceObject = new OpenGLVertexBufferDeviceObject();

	// Create OpenGL-side vertex buffer object
	GLuint vertexBufferID;
	GL_CALL(glGenBuffers(1, &vertexBufferID));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GL_CALL(glObjectLabel(GL_BUFFER, vertexBufferID, deviceObjectName.size(), deviceObjectName.c_str()));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// Update device object settings
	vertexBufferDeviceObject->id = vertexBufferID;

	outVertexBufferDeviceObject = vertexBufferDeviceObject;
}

void OpenGLContext::vertexBuffer_destroyDeviceObject(VertexBufferDeviceObject*& outVertexBufferDeviceObject)
{
	OpenGLVertexBufferDeviceObject* vertexBufferDeviceObject = dynamic_cast<OpenGLVertexBufferDeviceObject*>(outVertexBufferDeviceObject);
	assert(vertexBufferDeviceObject);

	// Delete OpenGL-side index buffer object
	GL_CALL(glDeleteBuffers(1, &vertexBufferDeviceObject->id));

	// Free device object
	delete vertexBufferDeviceObject;
	
	outVertexBufferDeviceObject = nullptr;
}

void OpenGLContext::vertexBuffer_initializeVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObjectBase, const BufferUsage bufferUsage, const Vertex* vertices, const uint32 vertexCount)
{
	OpenGLVertexBufferDeviceObject* vertexBufferDeviceObject = dynamic_cast<OpenGLVertexBufferDeviceObject*>(vertexBufferDeviceObjectBase);
	assert(vertexBufferDeviceObject);

	assert(vertexCount > 0);

	// Copy over vertex data
	const VertexFormat vertexFormat = vertices->getFormat();
	uint8* vertexData = new uint8[vertexCount * vertexFormat.getVertexSizeInBytes()];
	for (uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData((char*)vertexData + i * vertexFormat.getVertexSizeInBytes());
	}

	// Upload vertex data to vertex buffer object
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferDeviceObject->id));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexFormat.getVertexSizeInBytes(), vertexData, toBufferUsage(bufferUsage)));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	delete[] vertexData;

	// Update device object settings
	vertexBufferDeviceObject->vertexCount = vertexCount;
	vertexBufferDeviceObject->vertexFormat = vertexFormat;
}

void OpenGLContext::vertexBuffer_modifyVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObjectBase, const uint32 startIndex, const Vertex* vertices, const uint32 vertexCount)
{
	OpenGLVertexBufferDeviceObject* vertexBufferDeviceObject = dynamic_cast<OpenGLVertexBufferDeviceObject*>(vertexBufferDeviceObjectBase);
	assert(vertexBufferDeviceObject);

	assert(vertexCount > 0);

	// Copy over vertex data
	const VertexFormat vertexFormat = vertices->getFormat();
	assert(vertexBufferDeviceObject->vertexFormat == vertexFormat);
	uint8* vertexData = new uint8[vertexCount * vertexFormat.getVertexSizeInBytes()];
	for (uint i = 0; i < vertexCount; ++i)
	{
		vertices[i].getData((char*)vertexData + i * vertexFormat.getVertexSizeInBytes());
	}

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferDeviceObject->id));
	GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, startIndex * vertexFormat.getVertexSizeInBytes(), vertexCount * vertexFormat.getVertexSizeInBytes(), vertexData));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	delete[] vertexData;
}

void OpenGLContext::vertexBuffer_bindVertexBuffer(VertexBufferDeviceObject* vertexBufferDeviceObjectBase)
{
	OpenGLVertexBufferDeviceObject* vertexBufferDeviceObject = dynamic_cast<OpenGLVertexBufferDeviceObject*>(vertexBufferDeviceObjectBase);
	assert(vertexBufferDeviceObject);

	// Bind vertex buffer object
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferDeviceObject->id));
}

/**************************************************
 * IndexBuffer API implementation                 *
 **************************************************/

void OpenGLContext::indexBuffer_createDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject, const string& deviceObjectName)
{
	OpenGLIndexBufferDeviceObject* indexBufferDeviceObject = new OpenGLIndexBufferDeviceObject();

	// Create OpenGL-side index buffer object
	GLuint indexBufferID;
	GL_CALL(glGenBuffers(1, &indexBufferID));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID));
	GL_CALL(glObjectLabel(GL_BUFFER, indexBufferID, deviceObjectName.size(), deviceObjectName.c_str()));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	// Update device object settings
	indexBufferDeviceObject->id = indexBufferID;

	outIndexBufferDeviceObject = indexBufferDeviceObject;
}

void OpenGLContext::indexBuffer_destroyDeviceObject(IndexBufferDeviceObject*& outIndexBufferDeviceObject)
{
	OpenGLIndexBufferDeviceObject* indexBufferDeviceObject = dynamic_cast<OpenGLIndexBufferDeviceObject*>(outIndexBufferDeviceObject);
	assert(indexBufferDeviceObject);

	// Delete OpenGL-side index buffer object
	GL_CALL(glDeleteBuffers(1, &indexBufferDeviceObject->id));

	// Free device object
	delete indexBufferDeviceObject;

	outIndexBufferDeviceObject = nullptr;
}

void OpenGLContext::indexBuffer_initializeIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObjectBase, const BufferUsage bufferUsage, const uint32* indices, const uint32 indexCount)
{
	OpenGLIndexBufferDeviceObject* indexBufferDeviceObject = dynamic_cast<OpenGLIndexBufferDeviceObject*>(indexBufferDeviceObjectBase);
	assert(indexBufferDeviceObject);

	assert(indexCount > 0);
	assert(indices == nullptr);

	// Upload index data to index buffer object
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferDeviceObject->id));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * 4, indices, toBufferUsage(bufferUsage)));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	// Update device object settings
	indexBufferDeviceObject->indexCount = indexCount;
}

void OpenGLContext::indexBuffer_modifyIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObjectBase, const uint32 startIndex, const uint32* indices, const uint32 indexCount)
{
	OpenGLIndexBufferDeviceObject* indexBufferDeviceObject = dynamic_cast<OpenGLIndexBufferDeviceObject*>(indexBufferDeviceObjectBase);
	assert(indexBufferDeviceObject);

	assert(indexCount > 0);

	// Upload index data to index buffer object
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferDeviceObject->id));
	GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startIndex * 4, indexCount * 4, indices));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void OpenGLContext::indexBuffer_bindIndexBuffer(IndexBufferDeviceObject* indexBufferDeviceObjectBase)
{
	OpenGLIndexBufferDeviceObject* indexBufferDeviceObject = dynamic_cast<OpenGLIndexBufferDeviceObject*>(indexBufferDeviceObjectBase);
	assert(indexBufferDeviceObject);

	// Bind index buffer object
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferDeviceObject->id));
}

END_SAUCE_NAMESPACE
