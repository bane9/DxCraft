#pragma once
#include <math.h>
#include <DirectXMath.h>
#include "Position.h"

inline constexpr float PI = 3.14159265f;
inline constexpr double PI_D = 3.1415926535897932;

template <typename T>
inline constexpr auto sq(T x) noexcept
{
	return x * x;
}

template<typename T>
inline constexpr T wrap_angle(T theta) noexcept
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return modded > (T)PI_D ? modded - (T)2.0 * (T)PI_D : modded;
}

template<typename T>
inline constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

template<typename T>
inline constexpr T to_rad(T deg) noexcept
{
	return deg * (PI / (T)180.0);
}

template<typename T>
inline constexpr T FixedMod(T a, T b) noexcept
{ 
	return (a % b + b) % b; 
}

template<typename T>
inline constexpr T sgn(T n) noexcept
{
	return ((T)0.0 < n) - ((T)n < 0.0);
}

template<typename T, typename V>
inline constexpr float VectorDistance(const T& first, const V& second) noexcept
{
	return sqrtf(sq(second.x - first.x) + sq(second.y - first.y) + sq(second.z - first.z));
}

template<typename T>
inline constexpr float VectorLength(const T& n) noexcept
{
	return sqrtf(sq(n.x) + sq(n.y) + sq(n.z));
}

template<typename T>
inline constexpr T Normalize3D(const T& n) noexcept
{
	float length = VectorLength(n);
	return { n.x / length, n.y / length, n.z / length };
}

template<typename T, typename V>
inline constexpr float Dot3D(const T& n1, const V& n2) noexcept
{
	return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

template<typename T, typename V>
inline constexpr float PointDistance3D(const T& n1, const V& n2) noexcept
{
	return powf(sq(n2.x - n1.x) + sq(n2.y - n1.y) + sq(n2.z - n1.z), 0.5f);
}

template<typename T>
inline constexpr float BilinearInterpolation(T q11, T q12, T q21, T q22, T x1, T x2, T y1, T y2, T x, T y) noexcept
{
	return (T)1.0 /
		((x2 - x1) * (y2 - y1)) * (
		q11 * (x2 - x) * (y2 - y) +
		q21 * (x - x1) * (y2 - y) +
		q12 * (x2 - x) * (y - y1) +
		q22 * (x - x1) * (y - y1));
}

template<typename T, typename V>
inline constexpr V ValueMap(T val, T valMin, T valMax, V mapMin, V mapMax) noexcept 
{
	return (val - valMin) * (mapMax - mapMin) / (valMax - valMin) + mapMin;
}

template<typename T>
inline constexpr T Lerp(T a, T b, T f) noexcept
{
	return (a * (1.0f - f)) + (b * f);
}
