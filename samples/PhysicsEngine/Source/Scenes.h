#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

#include "Body.h"

const int g_benchmarkSceneNumCircles = 100; // Release, best: 400

class SceneManager
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

	void initialize(const ExampleScene scene);
	void clear();

	vector<Body*> &getBodies()
	{
		return m_bodies;
	}

private:
	void initializeEnclosureScene();
	void initializeCirclesBenchmarkScene();

	vector<Body*> m_bodies;
};
