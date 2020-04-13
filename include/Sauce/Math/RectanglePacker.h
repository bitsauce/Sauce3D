// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>

#include <Sauce/Math/Vector.h>
#include <Sauce/Math/Rectangle.h>

BEGIN_SAUCE_NAMESPACE

class SAUCE_API RectanglePacker
{
	friend class TextureAtlas;
public:
	RectanglePacker() :
		m_maxWidth(2048)
	{
	}

	void setMaxWidth(const int width)
	{
		m_maxWidth = width;
	}


	class SAUCE_API Entry : public Rect<uint>
	{
		friend class RectanglePacker;
	public:
		Entry()
			: Rect(0, 0, 0, 0)
			, valid(false)
			, key("")
			, data(nullptr)
		{
		}

		Entry(const string key, const uint width, const uint height, void *data)
			: Rect(0, 0, width, height)
			, valid(true)
			, key(key)
			, data(data)
		{
		}

		Entry(const Entry &other) :
			Rect(other),
			valid(other.valid),
			key(other.key),
			data(other.data)
		{
		}

		const void *getData() const
		{
			return data;
		}

	private:
		bool valid;
		string key;
		void *data;
	};

	class SAUCE_API Result
	{
	public:
		Result() :
			valid(false),
			canvas(0),
			area(0),
			efficiency(0.0f)
		{
		}

		void clear()
		{
			canvas.set(0, 0);
			area = 0;
			efficiency = 0.0f;
			rectangles.clear();
		}

		bool valid;
		Vector2I canvas;
		int area;
		float efficiency;
		map<string, Entry> rectangles;
	};

	const Result pack();
	void addRectangle(const string &key, const uint width, const uint height, void *data = 0);
	void clear();

private:
	vector<Entry> m_rectangles;
	int m_maxWidth;
};

END_SAUCE_NAMESPACE
