#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

class Body;
struct AABB;

class PhysicsGrid
{
public:
	void initialize(Vector2I numCells);
	void addBody(Body *body);
	void removeBody(Body *body);
	void bodyMoved(Body *body, AABB aabbBefore);

	list<Body*> getBodiesInCell(Vector2I coords)
	{
		return m_physicsGrid[coords.y][coords.x];
	}

	Vector2I getNumCells() const
	{
		return m_numCells;
	}

private:
	Vector2I m_numCells;

	// 2D grid containing a 1D array
	vector<vector<list<Body*>>> m_physicsGrid;
};
