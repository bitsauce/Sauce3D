#include "PhysicsGrid.h"
#include "Constants.h"
#include "Body.h"

void PhysicsGrid::initialize(Vector2I numCells)
{
	m_numCells = numCells;
	m_physicsGrid.resize(numCells.y);
	for(int y = 0; y < numCells.y; ++y)
	{
		m_physicsGrid[y].resize(numCells.x);
		for(int x = 0; x < numCells.x; ++x)
		{
			//m_physicsGrid[y][x].reserve(g_initialBodiesPerCell);
		}
	}
}

void PhysicsGrid::addBody(Body *body)
{
	const AABB aabb = body->getAABB();
	const Vector2I min = math::floor(aabb.min / g_physicsCellSize);
	const Vector2I max = math::floor(aabb.max / g_physicsCellSize);
	for(int y = math::maximum(min.y, 0); y <= math::minimum(max.y, m_numCells.y-1); ++y)
	{
		for(int x = math::maximum(min.x, 0); x <= math::minimum(max.x, m_numCells.x-1); ++x)
		{
			m_physicsGrid[y][x].push_back(body);
		}
	}
}

void PhysicsGrid::removeBody(Body *body)
{
	const AABB aabb = body->getAABB();
	const Vector2I min = math::floor(aabb.min / g_physicsCellSize);
	const Vector2I max = math::floor(aabb.max / g_physicsCellSize);
	for(int y = math::maximum(min.y, 0); y <= math::minimum(max.y, m_numCells.y-1); ++y)
	{
		for(int x = math::maximum(min.x, 0); x <= math::minimum(max.x, m_numCells.x-1); ++x)
		{
			m_physicsGrid[y][x].remove(body);
		}
	}
}

AABB aabbToPhysicsGrid(const AABB &aabbIn)
{
	AABB aabb;
	aabb.min = math::floor(aabbIn.min / g_physicsCellSize);
	aabb.max = math::floor(aabbIn.max / g_physicsCellSize);
	return aabb;
}

void PhysicsGrid::bodyMoved(Body *body, AABB aabbBefore)
{
	aabbBefore = aabbToPhysicsGrid(aabbBefore);
	const AABB aabbAfter = aabbToPhysicsGrid(body->getAABB());
	const bool overlap = aabbBefore.overlaps(aabbAfter);
	const AABB intersection = aabbAfter.intersection(aabbBefore);

	// Call remove on cells in aabbBefore but not in intersection
	for(int y = math::maximum(aabbBefore.min.y, 0); y <= math::minimum(aabbBefore.max.y, m_numCells.y-1); ++y)
	{
		for(int x = math::maximum(aabbBefore.min.x, 0); x <= math::minimum(aabbBefore.max.x, m_numCells.x-1); ++x)
		{
			if(overlap && intersection.contains(Vector2F(x, y)))
				continue;
			m_physicsGrid[y][x].remove(body);
		}
	}

	// Call add on cells in aabbAfter but not in intersection
	for(int y = math::maximum(aabbAfter.min.y, 0); y <= math::minimum(aabbAfter.max.y, m_numCells.y-1); ++y)
	{
		for(int x = math::maximum(aabbAfter.min.x, 0); x <= math::minimum(aabbAfter.max.x, m_numCells.x-1); ++x)
		{
			if(overlap && intersection.contains(Vector2F(x, y)))
				continue;
			m_physicsGrid[y][x].push_back(body);
		}
	}
}
