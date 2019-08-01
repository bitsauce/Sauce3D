#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

class Shape
{
public:
	enum Type
	{
		BOX,
		CIRCLE,
		NUM_SHAPES
	};

protected:
	Shape(Type type) :
		m_type(type),
		m_restitution(0.5f)
	{
		setMass(1.0f);
	}

public:
	bool m_isColliding = false;

	virtual Vector2F getCenter() const = 0;
	virtual void setCenter(Vector2F center) = 0;
	virtual void move(Vector2F deltaPosition) = 0;
	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	Type getType() const { return m_type; }

	float getMass() const { return m_mass; }
	float getMassInv() const { return m_massInv; }
	void setMass(const float mass) { m_mass = mass; m_massInv = mass > 0.0f ? 1.0f / mass : 0.0f; }
	bool isStatic() const { return m_mass <= 0.0f; }

	float getRestitution() const { return m_restitution; }
	float setRestitution(const float restitution) { m_restitution = restitution; }

	Vector2F getVelocity() const { return m_velocity; }
	void setVelocity(const Vector2F velocity) { m_velocity = velocity; }

	float staticFriction = 0.2f;
	float dynamicFriction = 0.1f;

private:
	const Type m_type;
	float m_mass, m_massInv;
	Vector2F m_velocity;
	float m_restitution;
};

class Box : public Shape
{
public:
	Box() :
		Shape(BOX),
		min(0.0f, 0.0f),
		max(10.0f, 10.0f)
	{
	}

	Vector2F getCenter() const override
	{
		return (min + max) / 2;
	}

	void setCenter(Vector2F center) override
	{
		Vector2F halfSize = getSize() * 0.5f;
		min = center - halfSize;
		max = center + halfSize;
	}

	void move(Vector2F deltaPosition) override
	{
		min += deltaPosition;
		max += deltaPosition;
	}

	void draw(GraphicsContext *graphicsContext, Color color) const override
	{
		graphicsContext->drawRectangle(min, max - min, color);
	}

	bool contains(Vector2F point) const override
	{
		return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
	}

	Vector2F getMin() const { return min; }
	Vector2F getMax() const { return max; }
	Vector2F getSize() const { return max - min; }
	void setSize(Vector2F size)
	{
		Vector2F center = getCenter();
		min = Vector2F(0.0f, 0.0f);
		max = size;
		setCenter(center);
	}

private:
	Vector2F min, max;
};

class Circle : public Shape
{
public:
	Circle() :
		Shape(CIRCLE),
		position(0.0f),
		radius(10.0f)
	{
	}
	
	Vector2F getCenter() const override
	{
		return position;
	}
	
	void setCenter(Vector2F center) override
	{
		position = center;
	}
	
	void move(Vector2F deltaPosition) override
	{
		position += deltaPosition;
	}

	void draw(GraphicsContext *graphicsContext, Color color) const override
	{
		graphicsContext->drawCircle(position, radius, 32, color);
	}

	bool contains(Vector2F point) const override
	{
		return (position - point).lengthSquared() < radius * radius;
	}

	float getRadius() const { return radius; }
	void setRadius(const float rad) { radius = rad; }

private:
	Vector2F position;
	float radius;
};
