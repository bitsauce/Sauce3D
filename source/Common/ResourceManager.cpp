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
#include <Sauce/Utils.h>
#include <Sauce/Graphics/Texture.h>
#include <Sauce/Graphics/FontRendering.h>
#include <Sauce/Graphics/Shader.h>

BEGIN_SAUCE_NAMESPACE
//
//ResourceManager *ResourceManager::s_this = 0;
//
//ResourceManager::ResourceManager(const string &resourceFile)
//{
//	if(s_this) THROW("ResourceManager::s_this != 0");
//	s_this = this;
//
//	// Load resource file
//	if(util::fileExists(resourceFile))
//	{
//		tinyxml2::XMLDocument doc;
//		doc.LoadFile(resourceFile.c_str());
//
//		// Get root node
//		tinyxml2::XMLNode *resourceNode = doc.FirstChildElement();
//		if(!resourceNode)
//		{
//			LOG("Invalid resource file");
//			return;
//		}
//
//		// For each context
//		resourceNode = resourceNode->FirstChildElement();
//		while(resourceNode)
//		{
//			// For each resource entry
//			tinyxml2::XMLElement *name = resourceNode->FirstChildElement("name");
//
//			// Get name and path
//			if(name)
//			{
//				string type = resourceNode->Value();
//				//if(type == "texture")
//				//{
//				//	tinyxml2::XMLElement *path = resourceNode->FirstChildElement("path");
//				//	tinyxml2::XMLElement *premul = resourceNode->FirstChildElement("premultiplyAlpha");
//				//	if(path)
//				//	{
//				//		m_resourceDesc[name->GetText()] =
//				//			new TextureResourceDesc(
//				//				name->GetText(),
//				//				path->GetText(),
//				//				premul && string(premul->GetText()) == "true"
//				//				);
//				//	}
//				//}
//				//else if(type == "font") // TODO: Fonts
//				//{
//				//	tinyxml2::XMLElement *path = resourceNode->FirstChildElement("path");
//				//	tinyxml2::XMLElement *premul = resourceNode->FirstChildElement("premultiplyAlpha");
//				//	if(path)
//				//	{
//				//		m_resourceDesc[name->GetText()] = new FontResourceDesc(
//				//			name->GetText(),
//				//			path->GetText(),
//				//			premul && string(premul->GetText()) == "true"
//				//			);
//				//	}
//				//}
//				//else if(type == "shader")
//				//{
//				//	tinyxml2::XMLElement *vertexFilePath = resourceNode->FirstChildElement("vertexFilePath");
//				//	tinyxml2::XMLElement *fragmentFilePath = resourceNode->FirstChildElement("fragmentFilePath");
//				//	tinyxml2::XMLElement *geometryFilePath = resourceNode->FirstChildElement("geometryFilePath");
//				//	if(vertexFilePath && fragmentFilePath)
//				//	{
//				//		m_resourceDesc[name->GetText()] = new ShaderResourceDesc(name->GetText(), vertexFilePath->GetText(), fragmentFilePath->GetText(), geometryFilePath ? geometryFilePath->GetText() : "");
//				//	}
//				//}
//			}
//
//			// Next resource
//			resourceNode = resourceNode->NextSibling();
//		}
//	}
//}

END_SAUCE_NAMESPACE
