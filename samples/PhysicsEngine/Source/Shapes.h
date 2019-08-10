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

	Shape(Type type) :
		m_type(type),
		m_relativePosition(0.0f, 0.0f)
	{
	}

	// TODO: Rename bodyLocalPosition?
	void setRelativePosition(Vector2F relativePosition)
	{
		m_relativePosition = relativePosition;
	}

	Vector2F getRelativePosition() const
	{
		return m_relativePosition;
	}

	virtual void draw(GraphicsContext *graphicsContext, Color color) const = 0;
	virtual bool contains(Vector2F point) const = 0;

	Type getType() const { return m_type; }

	list<pair<Vector2F, Color>> debugPoints;

private:
	const Type m_type;
	Vector2F m_relativePosition;
};

class Box : public Shape
{
public:
	Box() :
		Shape(BOX),
		m_size(10.0f, 10.0f)
	{
		m_normals[0] = Vector2F( 0.0f, -1.0f); // Top
		m_normals[1] = Vector2F( 1.0f,  0.0f); // Right
		m_normals[2] = Vector2F( 0.0f,  1.0f); // Bottom
		m_normals[3] = Vector2F(-1.0f,  0.0f); // Left
	}

	//Vector2F getCenter() const override
	//{
	//	return (min + max) / 2;
	//}

	//void setCenter(Vector2F center) override
	//{
	//	Vector2F halfSize = getSize() * 0.5f;
	//	min = center - halfSize;
	//	max = center + halfSize;
	//}

	//void move(Vector2F deltaPosition) override
	//{
	//	min += deltaPosition;
	//	max += deltaPosition;
	//}

	void draw(GraphicsContext *graphicsContext, Color color) const override
	{
		static Vertex vertices[4];
		static uint indices[8] = {
			0, 1,
			1, 3,
			3, 2,
			2, 0
		};

		Vector2F position = getRelativePosition();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		Matrix4 mat;
		mat.scale(size.x, size.y, 1.0f);
 		mat.translate(-halfSize.x, -halfSize.y, 0.0f);
		//mat.rotateZ(math::radToDeg(m_angle));
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
		getTransformedNormals(transformedNormals, Matrix4());
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
		const Vector2F halfSize = getSize() * 0.5f;
		const Vector2F min = getRelativePosition() - halfSize;
		const Vector2F max = getRelativePosition() + halfSize;
		return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
	}

	Vector2F getSize() const { return m_size; }
	void setSize(const Vector2F size) { m_size = size; }

	//Polygon *getPolygon() const
	void getPolygon(PhysicsPolygon *polygon, Matrix4 normalTransform = Matrix4(), Matrix4 pointTransform = Matrix4()) const
	{
		Vector2F normals[4], corners[4];
		getTransformedNormals(normals, normalTransform);
		getCornerVectors(corners, normalTransform);
		swap(corners[2], corners[3]);
		polygon->init(corners, pointTransform * getRelativePosition(), normals, 4);
	}

	void getTransformedNormals(Vector2F *transformedNormals, Matrix4 normalTransform) const
	{
		Vector2F position = getRelativePosition();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		//Matrix4 mat;
		//mat.rotateZ(math::radToDeg(m_angle));

		for(int i = 0; i < 4; i++)
		{
			transformedNormals[i] = normalTransform * m_normals[i];
		}
	}

	void getCornerVectors(Vector2F *cornerVectors, Matrix4 pointTransform) const
	{
		Vector2F position = getRelativePosition();
		Vector2F size = getSize();
		Vector2F halfSize = size * 0.5f;

		Matrix4 mat;
		mat.scale(size.x, size.y, 1.0f);
		mat.translate(-halfSize.x, -halfSize.y, 0.0f);
		//mat.rotateZ(math::radToDeg(m_angle));
		//mat.rotateZ(math::radToDeg(45.0f));

		mat = pointTransform * mat;

		for(int i = 0; i < 4; i++)
		{
			cornerVectors[i] = (mat * QUAD_VERTICES[i]).getXY();
		}
	}

private:
	Vector2F m_size;
	Vector2F m_normals[4];
};

//class Circle : public Shape
//{
//public:
//	Circle() :
//		Shape(CIRCLE),
//		position(0.0f),
//		radius(10.0f)
//	{
//	}
//	
//	Vector2F getCenter() const override
//	{
//		return position;
//	}
//	
//	void setCenter(Vector2F center) override
//	{
//		position = center;
//	}
//	
//	void move(Vector2F deltaPosition) override
//	{
//		position += deltaPosition;
//	}
//
//	void draw(GraphicsContext *graphicsContext, Color color) const override
//	{
//		graphicsContext->drawCircle(position, radius, 32, color);
//	}
//
//	bool contains(Vector2F point) const override
//	{
//		return (position - point).lengthSquared() < radius * radius;
//	}
//
//	float getRadius() const { return radius; }
//	void setRadius(const float rad) { radius = rad; }
//
//private:
//	Vector2F position;
//	float radius;
//};
