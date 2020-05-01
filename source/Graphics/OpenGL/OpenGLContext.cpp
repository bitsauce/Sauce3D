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
#include <Sauce/Graphics/OpenGL/OpenGLRenderTarget.h>
#include <Sauce/Graphics/OpenGL/OpenGLTexture.h>
#include <Sauce/Graphics/OpenGL/OpenGLShader.h>

BEGIN_SAUCE_NAMESPACE

#ifdef SAUCE_DEBUG
void checkOpenGLError(const string &info)
{
	GLenum error;
	if((error = glGetError()) != GL_NO_ERROR)
	{
		string errorType = "";
		switch(error)
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

// Global GL objects for easy rendering
GLuint OpenGLContext::s_vao = 0;
GLuint OpenGLContext::s_vbo = 0;
GLuint OpenGLContext::s_ibo = 0;

OpenGLContext::OpenGLContext(const int major, const int minor) :
	m_majorVersion(major),
	m_minorVersion(minor)
{
}

OpenGLContext::~OpenGLContext()
{
	GL_CALL(glDeleteBuffers(1, &s_vbo));
	GL_CALL(glDeleteVertexArrays(1, &s_vao));
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

void OpenGLContext::clear(const uint mask, const Color &fillColor)
{
	if(mask & BufferMask::COLOR_BUFFER)   GL_CALL(glClearColor(fillColor.getR() / 255.0f, fillColor.getG() / 255.0f, fillColor.getB() / 255.0f, fillColor.getA() / 255.0f));
	if(mask & BufferMask::DEPTH_BUFFER)   GL_CALL(glClearDepth(fillColor.getR() / 255.0f));
	if(mask & BufferMask::STENCIL_BUFFER) GL_CALL(glClearStencil(fillColor.getR() / 255.0f));
	                                      GL_CALL(glClear(mask));
	if(mask & BufferMask::COLOR_BUFFER)   GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	if(mask & BufferMask::DEPTH_BUFFER)   GL_CALL(glClearDepth(0.0f));
	if(mask & BufferMask::STENCIL_BUFFER) GL_CALL(glClearStencil(0.0f));
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
	GL_CALL(glGenVertexArrays(1, &s_vao));
	GL_CALL(glBindVertexArray(s_vao));
	GL_CALL(glGenBuffers(1, &s_vbo));
	GL_CALL(glGenBuffers(1, &s_ibo));

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

	// Create passthrough shader
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

	const string fragmentShader =
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

	OpenGLShader::s_glslVersion = getGLSLVersion();
	s_defaultShader = ShaderRef(new OpenGLShader(vertexShader, fragmentShader, ""));

	// Create blank texture
	uint8 pixel[4];
	pixel[0] = pixel[1] = pixel[2] = pixel[3] = 255;
	s_defaultTexture = Texture2DRef(GraphicsContext::createTexture(1, 1, PixelFormat(PixelComponents::RGBA, PixelDatatype::UNSIGNED_BYTE), pixel));

	return m_window;
}

void OpenGLContext::setupContext()
{
	// Set blend func
	GL_CALL(glBlendFuncSeparate(
		(uint32)m_currentState->blendState.m_src,
		(uint32)m_currentState->blendState.m_dst,
		(uint32)m_currentState->blendState.m_alphaSrc,
		(uint32)m_currentState->blendState.m_alphaDst)
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

	OpenGLShader *glShader = dynamic_cast<OpenGLShader*>(shader.get());

	// Enable shader
	GL_CALL(glUseProgram(glShader->m_id));

	// Set projection matrix
	Matrix4 modelViewProjection = m_currentState->projectionMatrix * m_currentState->transformationMatrixStack.top();
	shader->setUniformMatrix4f("u_ModelViewProj", modelViewProjection.get());

	GLuint target = 0;

	// Set all uniforms
	for(map<string, OpenGLShader::Uniform*>::iterator itr = glShader->m_uniforms.begin(); itr != glShader->m_uniforms.end(); ++itr)
	{
		const OpenGLShader::Uniform *uniform = itr->second;
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
				GL_CALL(glActiveTexture(GL_TEXTURE0 + target));
				GL_CALL(glBindTexture(GL_TEXTURE_2D, ((GLuint*) uniform->data)[0]));
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
					GL_CALL(glVertexAttribPointer(0, fmt.getElementCount(attrib), (uint32)fmt.getDatatype(attrib), GL_TRUE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
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
					GL_CALL(glVertexAttribPointer(1, fmt.getElementCount(attrib), (uint32)fmt.getDatatype(attrib), GL_TRUE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
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
					GL_CALL(glVertexAttribPointer(2, fmt.getElementCount(attrib), (uint32)fmt.getDatatype(attrib), GL_FALSE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
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
					GL_CALL(glVertexAttribPointer(3, fmt.getElementCount(attrib), (uint32)fmt.getDatatype(attrib), GL_FALSE, vertexSizeInBytes, (void*)(uint64)fmt.getAttributeOffset(attrib)));
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

void OpenGLContext::drawIndexedPrimitives(const PrimitiveType type, const Vertex *vertices, const uint vertexCount, const uint *indices, const uint indexCount)
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
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, s_vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSizeInBytes, vertexData, GL_DYNAMIC_DRAW));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint), indices, GL_DYNAMIC_DRAW));

	// Setup vertex attribute pointers
	setupVertexAttributePointers(fmt);

	// Draw primitives
	GL_CALL(glDrawElements((uint32)type, indexCount, GL_UNSIGNED_INT, 0));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	// Release vertex data
	delete[] vertexData;
}

void OpenGLContext::drawIndexedPrimitives(const PrimitiveType type, const VertexBuffer *vbo, const IndexBuffer *ibo)
{
	// If one of the buffers are empty, do nothing
	if(vbo->getSize() == 0 || ibo->getSize() == 0) return; 

	setupContext();

	// Bind vertices and indices array
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo->m_id));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->m_id));

	// Setup vertex attribute pointers
	const VertexFormat fmt = vbo->getVertexFormat();
	setupVertexAttributePointers(fmt);

	// Draw vbo
	GL_CALL(glDrawElements((uint32)type, ibo->getSize(), GL_UNSIGNED_INT, 0));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void OpenGLContext::drawPrimitives(const PrimitiveType type, const Vertex *vertices, const uint vertexCount)
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
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, s_vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSizeInBytes, vertexData, GL_DYNAMIC_DRAW));

	// Setup vertex attribute pointers
	setupVertexAttributePointers(fmt);

	// Draw primitives
	GL_CALL(glDrawArrays((uint32)type, 0, vertexCount));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// Release vertex data
	delete[] vertexData;
}

void OpenGLContext::drawPrimitives(const PrimitiveType type, const VertexBuffer *vbo)
{
	// If the buffer is empty, do nothing
	if(vbo->getSize() == 0) return;

	setupContext();

	// Bind vertices and indices array
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo->m_id));

	// Setup vertex attribute pointers
	const VertexFormat fmt = vbo->getVertexFormat();
	setupVertexAttributePointers(fmt);

	// Draw vbo
	GL_CALL(glDrawArrays((uint32)type, 0, vbo->getSize()));

	// Reset vbo buffers
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

RenderTarget2D *OpenGLContext::createRenderTarget(const uint width, const uint height, const uint targetCount, const PixelFormat &format)
{
	return static_cast<RenderTarget2D*>(new OpenGLRenderTarget2D(this, width, height, targetCount, format));
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

Texture2D *OpenGLContext::createTexture(const Pixmap &pixmap)
{
	return static_cast<Texture2D*>(new OpenGLTexture2D(pixmap));
}

Shader *OpenGLContext::createShader(const string &vertexSource, const string &fragmentSource, const string &geometrySource)
{
	return static_cast<Shader*>(new OpenGLShader(vertexSource, fragmentSource, geometrySource));
}

END_SAUCE_NAMESPACE
