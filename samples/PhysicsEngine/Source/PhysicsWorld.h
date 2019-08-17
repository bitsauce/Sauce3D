#pragma once

#include "Config.h"
#include "PhysicsGrid.h"
#include "Scenes.h"

struct ManifoldKey;
class Manifold;
class Body;

class PhysicsWorld
{

	// Manages the spatial division of physics bodies
	PhysicsGrid m_physicsGrid;

	// List of all bodies
	list<Body*> m_bodies;

	// Manifold generation table
	function<void(Manifold*)> m_manifoldGenerationFunctionTable[Shape::NUM_SHAPES][Shape::NUM_SHAPES];

	// Map that manages manifolds across updates
	map<ManifoldKey, Manifold> m_manifolds;
	typedef map<ManifoldKey, Manifold>::iterator ManifoldItr;

	// Time scale variable
	float m_timeScale;

public:
	PhysicsWorld();

	void initialize();

	void broadphase();

	void update(float dt);

	void preStep(const float dt, Manifold *m);
	void resolveCollision(Manifold *m);
	void correctPositions(Manifold *m);

	list<Body*> &getBodies() { return m_bodies; }
	PhysicsGrid *getPhysicsGrid() { return &m_physicsGrid; }
};
