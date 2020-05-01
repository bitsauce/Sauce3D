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
{
}

Shader::~Shader()
{
}

void *ShaderResourceDesc::create() const
{
	GraphicsContext *graphicsContext = Game::Get()->getWindow()->getGraphicsContext();
	stringstream vertexSource, fragmentSource, geometrySource;

	ifstream *fileReader;

	fileReader = new ifstream(util::getAbsoluteFilePath(m_vertexFilePath));
	vertexSource << fileReader->rdbuf();
	fileReader->close();
	delete fileReader;

	fileReader = new ifstream(util::getAbsoluteFilePath(m_fragmentFilePath));
	fragmentSource << fileReader->rdbuf();
	fileReader->close();
	delete fileReader;

	if(util::fileExists(m_geometryFilePath))
	{
		fileReader = new ifstream(util::getAbsoluteFilePath(m_geometryFilePath));
		geometrySource << fileReader->rdbuf();
		fileReader->close();
		delete fileReader;
	}

	LOG("Compiling shader program: %s", getName().c_str());

	return graphicsContext->createShader(vertexSource.str(), fragmentSource.str(), geometrySource.str());
}

END_SAUCE_NAMESPACE
