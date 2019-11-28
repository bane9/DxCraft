#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
inline constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

template<typename T>
inline constexpr T wrap_angle(T theta) noexcept
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return (modded > (T)PI_D) ?
		(modded - (T)2.0 * (T)PI_D) :
		modded;
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

inline constexpr int sgn(float n) noexcept
{
	return (0.0f < n) - (n < 0.0f);
}

template<typename T, typename V>
inline constexpr float VectorDistance(T first, V second) noexcept
{
	return sqrtf(powf(second.x - first.x, 2.0f) + powf(second.y - first.y, 2.0f) + powf(second.z - first.z, 2.0f));
}

template<typename T>
inline constexpr float VectorLength(T n) noexcept
{
	return sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
}

template<typename T>
inline constexpr T Normalize3D(T n) noexcept
{
	float length = VectorLength(n);
	return { n.x / length, n.y / length, n.z / length };
}

template<typename T, typename V>
inline constexpr float Dot3D(T n1, V n2) noexcept
{
	return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}
