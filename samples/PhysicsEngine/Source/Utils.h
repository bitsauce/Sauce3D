#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

namespace sauce
{
	namespace math
	{
		Vector2F perp(const Vector2F &v);
	}
}

struct AABB
{
	Vector2F min, max;

	bool overlaps(const AABB &other) const
	{
		return min.x <= other.max.x &&
			max.x >= other.min.x &&
			min.y <= other.max.y &&
			max.y >= other.min.y;
	}

	bool contains(const Vector2F &point) const
	{
		return point.x >= min.x &&
			point.x <= max.x &&
			point.y <= max.y &&
			point.y >= min.y;
	}

	AABB intersection(const AABB &other) const
	{
		AABB aabb;
		aabb.min.x = math::maximum(min.x, other.min.x);
		aabb.min.y = math::maximum(min.y, other.min.y);
		aabb.max.x = math::minimum(max.x, other.max.x);
		aabb.max.y = math::minimum(max.y, other.max.y);
		return aabb;
	}
};
