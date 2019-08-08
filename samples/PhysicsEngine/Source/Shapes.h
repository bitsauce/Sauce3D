#pragma once

#include <Sauce/Sauce.h>
using namespace sauce;

#include "Geometry.h"

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
		m_restitution(0.5f),
		m_angularVelocity(0.0f),
		m_angle((float(rand()) / RAND_MAX) * 0.01)
	{
		//setMass(1.0f);
		setMass(0.001f);
		setInertia(1.0f);
	}

public:
	bool m_isColliding = false;

	virtual Vector2F getCenter() const = 0;
	virtual void setCenter(Vector2F center) = 0;
	virtual void move(Vector2F deltaPosition) = 0;
	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	void rotate(const float angle)
	{
		m_angle += angle;
	}

	Type getType() const { return m_type; }

	float getMass() const { return m_mass; }
	float getMassInv() const { return m_massInv; }
	void setMass(const float mass) { m_mass = mass; m_massInv = mass > 0.0f ? 1.0f / mass : 0.0f; }
	bool isStatic() const { return m_mass <= 0.0f; }

	float getInertia() const { return m_inertia; }
	float getInertiaInv() const { return m_inertiaInv; }
	void setInertia(const float inertia) { m_inertia = inertia; m_inertiaInv = inertia > 0.0f ? 1.0f / inertia : 0.0f; }

	float getRestitution() const { return m_restitution; }
	float setRestitution(const float restitution) { m_restitution = restitution; }

	Vector2F getVelocity() const { return m_velocity; }
	void setVelocity(const Vector2F velocity) { m_velocity = velocity; }

	float getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(const float angularVelocity) { m_angularVelocity = angularVelocity; }

	void applyImpulse(const Vector2F &impulse, const Vector2F &radius)
	{
		m_velocity += impulse * m_massInv;
		m_angularVelocity += radius.cross(impulse) * m_inertiaInv;
	}

	float staticFriction = 0.2f;
	float dynamicFriction = 0.1f;

	float m_angle = 0.0f;

private:
	const Type m_type;
	float m_mass, m_massInv;
	float m_inertia, m_inertiaInv;
	Vector2F m_velocity;
	float m_angularVelocity;
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
		normals[0] = Vector2F( 0.0f, -1.0f); // Top
		normals[1] = Vector2F( 1.0f,  0.0f); // Right
		normals[2] = Vector2F( 0.0f,  1.0f); // Bottom
		normals[3] = Vector2F(-1.0f,  0.0f); // Left
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
		static Vertex vertices[4];
		static uint indices[8] = {
			0, 1,
			1, 3,
			3, 2,
			2, 0
		};

		//Matrix4 mat;
		//mat.scale(m_size.x, m_size.y, 1.0f);
		//mat.translate(-m_origin.x, -m_origin.y, 0.0f);
		//mat.scale(m_scale.x, m_scale.y, 1.0f);
		//mat.rotateZ(m_angle);
		//mat.translate(m_position.x, m_position.y, 0.0f);

		Vector2F position = getCenter();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		Matrix4 mat;
		mat.scale(size.x, size.y, 1.0f);
 		mat.translate(-halfSize.x, -halfSize.y, 0.0f);
		mat.rotateZ(math::radToDeg(m_angle));
		mat.translate(position.x, position.y, 0.0f);

		for(int i = 0; i < 4; i++)
		{
			Vector2F pos = (mat * QUAD_VERTICES[i]).getXY();
			vertices[i].set2f(VERTEX_POSITION, pos.x, pos.y);
			vertices[i].set4ub(VERTEX_COLOR, color.getR(), color.getG(), color.getB(), color.getA());
		}

		//vertices[0].set2f(VERTEX_TEX_COORD, m_textureRegion.uv0.x, m_textureRegion.uv0.y);
		//vertices[1].set2f(VERTEX_TEX_COORD, m_textureRegion.uv1.x, m_textureRegion.uv0.y);
		//vertices[2].set2f(VERTEX_TEX_COORD, m_textureRegion.uv0.x, m_textureRegion.uv1.y);
		//vertices[3].set2f(VERTEX_TEX_COORD, m_textureRegion.uv1.x, m_textureRegion.uv1.y);

		//graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_TRIANGLES, vertices, 4, QUAD_INDICES, 6);
		graphicsContext->drawIndexedPrimitives(GraphicsContext::PRIMITIVE_LINES, vertices, 4, indices, 8);


		Vector2F transformedNormals[4];
		getTransformedNormals(transformedNormals);
		for(int i = 0; i < 4; i++)
		{
			graphicsContext->drawArrow(position + transformedNormals[i] * halfSize, position + transformedNormals[i] * (halfSize + Vector2F(15.0f)), Color::Blue);
		}

		for(pair<Vector2F, Color> p : debugPoints)
		{
			Vertex v;
			v.set2f(VERTEX_POSITION, p.first.x, p.first.y);
			v.set4ub(VERTEX_COLOR, p.second.getR(), p.second.getG(), p.second.getB(), p.second.getA());
			graphicsContext->drawPrimitives(GraphicsContext::PRIMITIVE_POINTS, &v, 1);
		}
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

	//Polygon *getPolygon() const
	void getPolygon(PhysicsPolygon *polygon) const
	{
		Vector2F normals[4], corners[4];
		getTransformedNormals(normals);
		getCornerVectors(corners);
		swap(corners[2], corners[3]);
		polygon->init(corners, getCenter(), normals, 4);
	}

	void getTransformedNormals(Vector2F *transformedNormals) const
	{
		Vector2F position = getCenter();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		Matrix4 mat;
		//mat.scale(size.x, size.y, 1.0f);
		//mat.translate(-halfSize.x, -halfSize.y, 0.0f);
		mat.rotateZ(math::radToDeg(m_angle));
		//mat.translate(position.x, position.y, 0.0f);

		for(int i = 0; i < 4; i++)
		{
			transformedNormals[i] = (mat * Vector4F(normals[i].x, normals[i].y, 0,1)).getXY();
		}
	}

	void getCornerVectors(Vector2F *cornerVectors) const
	{
		Vector2F position = getCenter();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		Matrix4 mat;
		mat.scale(size.x, size.y, 1.0f);
		mat.translate(-halfSize.x, -halfSize.y, 0.0f);
		mat.rotateZ(math::radToDeg(m_angle));

		for(int i = 0; i < 4; i++)
		{
			cornerVectors[i] = (mat * QUAD_VERTICES[i]).getXY();
		}
	}

	list<pair<Vector2F, Color>> debugPoints;

private:
	Vector2F min, max;
	Vector2F normals[4];
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
