#ifndef VEC_3_H
#define VEC_3_H

#include <math.h>

template <class T, class RESULT = float>
struct Vec3
{
	T x, y, z;
	Vec3(T px = 0, T py = 0, T pz = 0) : x(px), y(py), z(pz) {}

	T X() const { return x;}
	T Y() const { return y;}
	T Z() const { return z;}

	void SetX(T tx) {x = tx;}
	void SetY(T ty) {y = ty;}
	void SetZ(T tz) {z = tz;}

    void operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vec3& operator*(T t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

	RESULT Length() const
	{
		return sqrt(SquaredLength());
	}

	RESULT SquaredLength() const
	{
		return x * x + y * y + z * z;
	}

	RESULT DotProduct(const Vec3<T, RESULT>& v)
	{
		return x * v.x    + y * v.y   + z * v.z;
	}

	Vec3<T, RESULT> CrossProduct(const Vec3<T, RESULT>& v)
	{
		return Vec3<T, RESULT>(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

	Vec3<T, RESULT> operator-(const Vec3<T, RESULT>& v)
	{
		return Vec3<T, RESULT>(x - v.z, y - v.y, z - v.z);
	}

	void Normalize()
	{
		RESULT length = Length();
		x /= length;
		y /= length;
		z /= length;
	}
};

template <class T>
Vec3<T> operator+(const Vec3<T>& a, const Vec3<T>& b)
{
	Vec3<T> res = a;
	res += b;
	return res;
}
typedef Vec3<float, float> Vec3f;
typedef Vec3<int, int> Vec3i;

#endif
