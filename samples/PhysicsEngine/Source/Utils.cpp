#include "Config.h"

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