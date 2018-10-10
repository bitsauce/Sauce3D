#include <Sauce/Math/Vector.h>

template<typename T> const Vector2<T> Vector2<T>::Zero = Vector2<T>(T(0));

const Color Color::White = Color(255, 255, 255);
const Color Color::Black = Color(0, 0, 0);
const Color Color::Red = Color(255, 0, 0);
const Color Color::Green = Color(0, 255, 0);
const Color Color::Blue = Color(0, 0, 255);
const Color Color::Yellow = Color(255, 255, 0);

Color::Color() :
	Color(0, 0, 0, 0)
{
}

Color::Color(const uchar r, const uchar g, const uchar b, const uchar a) :
	Vector4<uchar>(r, g, b, a)
{
}