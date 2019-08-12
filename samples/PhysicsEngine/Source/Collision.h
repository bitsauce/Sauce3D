#pragma once

#include "Manifold.h"

namespace collision
{
	void CircleToCircle(Manifold *m);
	void PolygonToCircle(Manifold *m);
	void CircleToPolygon(Manifold *m);
	void PolygonToPolygon(Manifold *m);
}
