#include "Utils.h"
#include "Constants.h"

namespace sauce
{
	namespace math
	{
		Vector2F perp(const Vector2F &v)
		{
			return Vector2F(-v.y, v.x);
		}
	}
}

Vector2F Vector2FInPhysicsSpace(const float x, const float y)
{
	return Vector2F(x, y) / g_physicsUnit;
}

Vector2F Vector2FInPhysicsSpace(const Vector2F &v)
{
	return v / g_physicsUnit;
}

float ValueInPhysicsSpace(const float x)
{
	return x / g_physicsUnit;
}
