#include "Box.h"

Box::Box(const BoxDef &boxDef) :
	PolygonShape(boxDef),
	m_size(boxDef.size),
	m_angle(boxDef.angle)
{
	if(m_angle == 0.0f)
	{
		const Vector2F halfSize = m_size * 0.5f;
		const Vector2F corners[4] = {
			 Vector2F(-halfSize.x, -halfSize.y),
			 Vector2F(+halfSize.x, -halfSize.y),
			 Vector2F(+halfSize.x, +halfSize.y),
			 Vector2F(-halfSize.x, +halfSize.y)
		};
		initialize(corners, 4);
	}
	else
	{
		Matrix4 rotation;
		rotation.rotateZ(math::radToDeg(m_angle));
		const Vector2F halfSize = m_size * 0.5f;
		const Vector2F corners[4] = {
			 rotation * Vector2F(-halfSize.x, -halfSize.y),
			 rotation * Vector2F(+halfSize.x, -halfSize.y),
			 rotation * Vector2F(+halfSize.x, +halfSize.y),
			 rotation * Vector2F(-halfSize.x, +halfSize.y)
		};
		initialize(corners, 4);
	}
}
