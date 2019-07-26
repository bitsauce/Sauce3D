#pragma once

#include <Sauce/Sauce.h>

struct DirectionalLight
{
	DirectionalLight() :
		direction(0.f, 0.f, 0.f),
		color(1.f, 1.f, 1.f)
	{
	}

	Vector3F direction;
	Vector3F color;
};

struct PointLight
{
	PointLight(Vector3F position = Vector3F(0.f, 0.f, 0.f),
			   Vector3F color = Vector3F(1.f, 1.f, 1.f),
			   float radius =  0.f) :
		position(position),
		color(color),
		radius(radius)
	{
	}

	Vector3F position;
	Vector3F color;
	float radius;
};

struct SpotLight
{
	SpotLight() :
		position(0.0f, 0.0f, 0.f),
		color(1.f, 1.f, 1.f),
		innerRadius(0.0f),
		outerRadius(0.0f)
	{
	}

	Vector3F position;
	Vector3F color;
	float innerRadius;
	float outerRadius;
};
