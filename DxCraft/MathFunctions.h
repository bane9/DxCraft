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
