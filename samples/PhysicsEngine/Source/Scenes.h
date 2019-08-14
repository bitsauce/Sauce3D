#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

#include "Body.h"

class PhysicsGrid;

class PhysicsScene
{
public:
	enum ExampleScene
	{
		SCENE_EMPTY,
		SCENE_ENCLOSURE,
		SCENE_BENCHMARK_CIRCLES,
		SCENE_BENCHMARK_BOXES,
		SCENE_BENCHMARK_POLYGONS,
		SCENE_BENCHMARK_ALL,
	};

	void initialize(const ExampleScene scene, list<Body*> &bodies, PhysicsGrid *physicsGrid);

private:
	void setupEnclosureScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupCirclesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupBoxesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
};
