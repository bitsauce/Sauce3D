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

#include <Sauce/Common.h>
#include <Sauce/Graphics.h>
#include <Sauce/Utils.h>

BEGIN_SAUCE_NAMESPACE

Shader::Shader()
	: m_graphicsContext(nullptr)
	, m_deviceObject(nullptr)
{
}

Shader::~Shader()
{
	m_graphicsContext->shader_destroyDeviceObject(m_deviceObject);
}

bool Shader::initialize(ShaderDesc shaderDesc)
{
	// Get vertex shader source
	stringstream vsSource;
	bool hasVertexShader = true;
	{
		if (!shaderDesc.shaderFileVS.empty() && util::fileExists(shaderDesc.shaderFileVS))
		{
			ifstream fileReader(util::getAbsoluteFilePath(shaderDesc.shaderFileVS));
			vsSource << fileReader.rdbuf();
			fileReader.close();
		}
		else if (!shaderDesc.shaderSourceVS.empty())
		{
			vsSource << shaderDesc.shaderSourceVS;
		}
		else
		{
			hasVertexShader = false;
		}
	}

	// Get pixel shader source
	stringstream psSource;
	bool hasPixelShader = true;
	{
		if (!shaderDesc.shaderFilePS.empty() && util::fileExists(shaderDesc.shaderFilePS))
		{
			ifstream fileReader(util::getAbsoluteFilePath(shaderDesc.shaderFilePS));
			psSource << fileReader.rdbuf();
			fileReader.close();
		}
		else if (!shaderDesc.shaderSourcePS.empty())
		{
			psSource << shaderDesc.shaderSourcePS;
		}
		else
		{
			hasPixelShader = false;
		}
	}

	// Get geometry shader source
	stringstream gsSource;
	bool hasGeometryShader = true;
	{
		if (!shaderDesc.shaderFileGS.empty() && util::fileExists(shaderDesc.shaderFileGS))
		{
			ifstream fileReader(util::getAbsoluteFilePath(shaderDesc.shaderFileGS));
			gsSource << fileReader.rdbuf();
			fileReader.close();
		}
		else if (!shaderDesc.shaderSourceGS.empty())
		{
			gsSource << shaderDesc.shaderSourceGS;
		}
		else
		{
			hasGeometryShader = false;
		}
	}

	// Verify that we have a VS and PS
	if (!hasVertexShader || !hasPixelShader)
	{
		return false;
	}

	// Get graphics context to use
	if (shaderDesc.graphicsContext)
	{
		m_graphicsContext = shaderDesc.graphicsContext;
	}
	else
	{
		m_graphicsContext = GraphicsContext::GetContext();
	}

	// Get debug name
	if (shaderDesc.debugName.empty())
	{
		static uint32 anonymousShaderCount = 0;
		shaderDesc.debugName = "Shader_" + to_string(anonymousShaderCount);
		anonymousShaderCount++;
	}

	//LOG("Compiling shader program: %s", getName().c_str());

	// Create and initialize shader device object
	m_graphicsContext->shader_createDeviceObject(m_deviceObject, shaderDesc.debugName);
	m_graphicsContext->shader_compileShader(m_deviceObject, vsSource.str(), psSource.str(), gsSource.str());

	return true;
}

/**
 * Signed integers
 */
void Shader::setUniform1i(const string& uniformName, const int32 v0)
{
	static int32 values[1];
	values[0] = v0;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 1, 1, &values);
}

void Shader::setUniform2i(const string& uniformName, const int32 v0, const int32 v1)
{
	static int32 values[2];
	values[0] = v0; values[1] = v1;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 2, 1, &values);
}

void Shader::setUniform3i(const string& uniformName, const int32 v0, const int32 v1, const int32 v2)
{
	static int32 values[3];
	values[0] = v0; values[1] = v1; values[2] = v2;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 3, 1, &values);
}

void Shader::setUniform4i(const string& uniformName, const int32 v0, const int32 v1, const int32 v2, const int32 v3)
{
	static int32 values[4];
	values[0] = v0; values[1] = v1; values[2] = v2; values[3] = v3;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 4, 1, &values);
}

void Shader::setUniform2i(const string& uniformName, const Vector2I& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 2, 1, (int32*)&vec);
}

void Shader::setUniform3i(const string& uniformName, const Vector3I& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 3, 1, (int32*)&vec);
}

void Shader::setUniform4i(const string& uniformName, const Vector4I& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 4, 1, (int32*)&vec);
}

void Shader::setUniformArray1i(const string& uniformName, const uint32 numElements, const int32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 1, numElements, v);
}

void Shader::setUniformArray2i(const string& uniformName, const uint32 numElements, const int32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 2, numElements, v);
}

void Shader::setUniformArray3i(const string& uniformName, const uint32 numElements, const int32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 3, numElements, v);
}

void Shader::setUniformArray4i(const string& uniformName, const uint32 numElements, const int32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Int32, 4, numElements, v);
}

/**
 * Unsigned integers
 */
void Shader::setUniform1ui(const string& uniformName, const uint32 v0)
{
	static uint32 values[4];
	values[0] = v0;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 1, 1, &values);
}

void Shader::setUniform2ui(const string& uniformName, const uint32 v0, const uint32 v1)
{
	static uint32 values[4];
	values[0] = v0; values[1] = v1;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 2, 1, &values);
}

void Shader::setUniform3ui(const string& uniformName, const uint32 v0, const uint32 v1, const uint32 v2)
{
	static uint32 values[4];
	values[0] = v0; values[1] = v1; values[2] = v2;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 3, 1, &values);
}

void Shader::setUniform4ui(const string& uniformName, const uint32 v0, const uint32 v1, const uint32 v2, const uint32 v3)
{
	static uint32 values[4];
	values[0] = v0; values[1] = v1; values[2] = v2; values[3] = v3;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 4, 1, &values);
}

void Shader::setUniform2ui(const string& uniformName, const Vector2U& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 2, 1, (uint32*)&vec);
}

void Shader::setUniform3ui(const string& uniformName, const Vector3U& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 3, 1, (uint32*)&vec);
}

void Shader::setUniform4ui(const string& uniformName, const Vector4U& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 4, 1, (uint32*)&vec);
}

void Shader::setUniformArray1ui(const string& uniformName, const uint32 numElements, const uint32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 1, numElements, v);
}

void Shader::setUniformArray2ui(const string& uniformName, const uint32 numElements, const uint32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 2, numElements, v);
}

void Shader::setUniformArray3ui(const string& uniformName, const uint32 numElements, const uint32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 3, numElements, v);
}

void Shader::setUniformArray4ui(const string& uniformName, const uint32 numElements, const uint32* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Uint32, 4, numElements, v);
}

/**
 * Floats
 */
void Shader::setUniform1f(const string& uniformName, const float v0)
{
	static float values[4];
	values[0] = v0;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 1, 1, &values);
}

void Shader::setUniform2f(const string& uniformName, const float v0, const float v1)
{
	static float values[4];
	values[0] = v0; values[1] = v1;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 2, 1, &values);
}

void Shader::setUniform3f(const string& uniformName, const float v0, const float v1, const float v2)
{
	static float values[4];
	values[0] = v0; values[1] = v1; values[2] = v2;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 3, 1, &values);
}

void Shader::setUniform4f(const string& uniformName, const float v0, const float v1, const float v2, const float v3)
{
	static float values[4];
	values[0] = v0; values[1] = v1; values[2] = v2; values[3] = v3;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 4, 1, &values);
}

void Shader::setUniform2f(const string& uniformName, const Vector2F& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 2, 1, (float*)&vec);
}

void Shader::setUniform3f(const string& uniformName, const Vector3F& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 3, 1, (float*)&vec);
}

void Shader::setUniform4f(const string& uniformName, const Vector4F& vec)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 4, 1, (float*)&vec);
}

void Shader::setUniform3f(const string& uniformName, const ColorRGB& color)
{
	static float values[3];
	values[0] = color.getR() / 255.0f;
	values[1] = color.getG() / 255.0f;
	values[2] = color.getB() / 255.0f;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 3, 1, &values);
}

void Shader::setUniform4f(const string& uniformName, const Color& color)
{
	static float values[4];
	values[0] = color.getR() / 255.0f;
	values[1] = color.getG() / 255.0f;
	values[2] = color.getB() / 255.0f;
	values[3] = color.getA() / 255.0f;
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 4, 1, &values);
}

void Shader::setUniformArray1f(const string& uniformName, const uint32 numElements, const float* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 1, numElements, v);
}

void Shader::setUniformArray2f(const string& uniformName, const uint32 numElements, const float* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 2, numElements, v);
}

void Shader::setUniformArray3f(const string& uniformName, const uint32 numElements, const float* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 3, numElements, v);
}

void Shader::setUniformArray4f(const string& uniformName, const uint32 numElements, const float* v)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Float, 4, numElements, v);
}

/**
 * Texture samplers
 */
void Shader::setSampler2D(const string& uniformName, Texture2DRef texture)
{
	m_graphicsContext->shader_setSampler2D(m_deviceObject, uniformName, texture);
}

/**
 * Matrix4
 */
void Shader::setUniformMatrix4f(const string& uniformName, const float* mat)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Matrix4, 16, 1, mat);
}

/**
 * Struct
 */
void Shader::setUniformStruct(const string& uniformName, const uint8* structData)
{
	m_graphicsContext->shader_setUniform(m_deviceObject, uniformName, Datatype::Struct, 0, 1, structData);
}

END_SAUCE_NAMESPACE
