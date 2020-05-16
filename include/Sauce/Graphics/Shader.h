// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Common.h>
#include <Sauce/Graphics/Texture.h>

BEGIN_SAUCE_NAMESPACE

SAUCE_FORWARD_DECLARE(GraphicsContext);

struct SAUCE_API ShaderDeviceObject
{
	virtual ~ShaderDeviceObject() { }
};

struct SAUCE_API ShaderDesc : public GraphicsDeviceObjectDesc
{
	string shaderFileVS;
	string shaderFilePS;
	string shaderFileGS;

	string shaderSourceVS;
	string shaderSourcePS;
	string shaderSourceGS;
};

class SAUCE_API Shader final : public SauceObject<Shader, ShaderDesc>
{
	friend class GraphicsContext;
	friend class ResourceManager;
public:
	Shader();
	virtual ~Shader();

	bool initialize(DescType) override;
	
	/**
	 * Signed integers
	 */
	void setUniform1i(const string& uniformName, const int32 v0);
	void setUniform2i(const string& uniformName, const int32 v0, const int32 v1);
	void setUniform3i(const string& uniformName, const int32 v0, const int32 v1, const int32 v2);
	void setUniform4i(const string& uniformName, const int32 v0, const int32 v1, const int32 v2, const int32 v3);

	void setUniform2i(const string& uniformName, const Vector2I& vec);
	void setUniform3i(const string& uniformName, const Vector3I& vec);
	void setUniform4i(const string& uniformName, const Vector4I& vec);

	void setUniformArray1i(const string &uniformName, const uint32 numElements, const int32 *v);
	void setUniformArray2i(const string &uniformName, const uint32 numElements, const int32 *v);
	void setUniformArray3i(const string &uniformName, const uint32 numElements, const int32 *v);
	void setUniformArray4i(const string &uniformName, const uint32 numElements, const int32 *v);

	/**
	 * Unsigned integers
	 */
	void setUniform1ui(const string& uniformName, const uint32 v0);
	void setUniform2ui(const string& uniformName, const uint32 v0, const uint32 v1);
	void setUniform3ui(const string& uniformName, const uint32 v0, const uint32 v1, const uint32 v2);
	void setUniform4ui(const string& uniformName, const uint32 v0, const uint32 v1, const uint32 v2, const uint32 v3);

	void setUniform2ui(const string& uniformName, const Vector2U& vec);
	void setUniform3ui(const string& uniformName, const Vector3U& vec);
	void setUniform4ui(const string& uniformName, const Vector4U& vec);

	void setUniformArray1ui(const string& uniformName, const uint32 numElements, const uint32* v);
	void setUniformArray2ui(const string& uniformName, const uint32 numElements, const uint32* v);
	void setUniformArray3ui(const string& uniformName, const uint32 numElements, const uint32* v);
	void setUniformArray4ui(const string& uniformName, const uint32 numElements, const uint32* v);

	/**
	 * Floats
	 */
	void setUniform1f(const string& uniformName, const float v0);
	void setUniform2f(const string& uniformName, const float v0, const float v1);
	void setUniform3f(const string& uniformName, const float v0, const float v1, const float v2);
	void setUniform4f(const string& uniformName, const float v0, const float v1, const float v2, const float v3);

	void setUniform2f(const string& uniformName, const Vector2F& vec);
	void setUniform3f(const string& uniformName, const Vector3F& vec);
	void setUniform4f(const string& uniformName, const Vector4F& vec);

	void setUniform3f(const string& uniformName, const ColorRGB& color);
	void setUniform4f(const string& uniformName, const Color& color);

	void setUniformArray1f(const string& uniformName, const uint32 numElements, const float* v);
	void setUniformArray2f(const string& uniformName, const uint32 numElements, const float* v);
	void setUniformArray3f(const string& uniformName, const uint32 numElements, const float* v);
	void setUniformArray4f(const string& uniformName, const uint32 numElements, const float* v);

	/**
	 * Texture samplers
	 */
	void setSampler2D(const string& uniformName, Texture2DRef texture);

	/**
	 * Matrix4
	 */
	void setUniformMatrix4f(const string& uniformName, const float* mat);

	/**
	 * Struct
	 */
	void setUniformStruct(const string& uniformName, const uint8* structData);

private:
	GraphicsContextRef m_graphicsContext;
	ShaderDeviceObject* m_deviceObject;
};
SAUCE_TYPEDEFS(Shader);

END_SAUCE_NAMESPACE

