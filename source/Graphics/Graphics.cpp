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

BEGIN_SAUCE_NAMESPACE

uint QUAD_INDICES[6] = {
	0, 2, 1,
	3, 1, 2
};

Vector4F QUAD_VERTICES[4] = {
	Vector4F(0.0f, 0.0f, 0.0f, 1.0f),
	Vector4F(1.0f, 0.0f, 0.0f, 1.0f),
	Vector4F(0.0f, 1.0f, 0.0f, 1.0f),
	Vector4F(1.0f, 1.0f, 0.0f, 1.0f)
};

Vector2F QUAD_TEXCOORD[4] = {
	Vector2F(0.0f, 0.0f),
	Vector2F(1.0f, 0.0f),
	Vector2F(0.0f, 1.0f),
	Vector2F(1.0f, 1.0f)
};

END_SAUCE_NAMESPACE
