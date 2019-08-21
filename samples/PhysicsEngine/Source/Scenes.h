#pragma once

#include "Config.h"
#include "Body.h"

class PhysicsGrid;

enum ExampleScene
{
	SCENE_EMPTY,
	SCENE_SINGLE_BOX,
	SCENE_VERTICAL_STACK,
	SCENE_COMPOUND_BODIES,
	SCENE_POLYGON_TEST,
	SCENE_SHAPES_TEST,
	SCENE_RESTITUTION_TEST,
	SCENE_BENCHMARK_CIRCLES,
	SCENE_BENCHMARK_BOXES,
	SCENE_BENCHMARK_POLYGONS,
	SCENE_BENCHMARK_ALL,
	SCENES_MAX
};

class SceneManager
{
public:
	void initialize(const ExampleScene scene, list<Body*> &bodies, PhysicsGrid *physicsGrid);

	ExampleScene m_currentScene;

private:
	void setupEnclosure(list<Body*> &bodies, PhysicsGrid *physicsGrid);

	void setupSingleBoxScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupVerticalStackScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupCompoundBodiesScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupPolygonTestScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupShapesTestScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupRestitutionTestScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);

	void setupBoxesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupCirclesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupPolygonBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
	void setupAllBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid);
};
