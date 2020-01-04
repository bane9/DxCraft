#pragma once
#include <DirectXMath.h>
#include <math.h>
#include "MathFunctions.h"
#include <algorithm>

class Ray {
public:
	Ray(float limit = 8.0f)
		: rayLimit(limit)
	{}

	void SetPositionAndDirection(const DirectX::XMFLOAT3& startCoord, float pitch, float yaw) noexcept {
		rayOrigin = startCoord;
		rayPos = startCoord;

		dx = std::sin(yaw) * std::cos(pitch);
		dy = std::sin(-pitch);
		dz = std::cos(yaw) * std::cos(pitch);

		rayDistance = 0.0f;

		falloff = 0.001f;
	}

	bool Next() noexcept
	{
		rayPos.x += dx * falloff;
		rayPos.y += dy * falloff;
		rayPos.z += dz * falloff;
		rayDistance += falloff;
		return rayDistance < rayLimit;
	}


	DirectX::XMFLOAT3 GetVector() const noexcept
	{
		return rayPos;
	}

	DirectX::XMFLOAT3 GetOrigin() const noexcept
	{
		return rayOrigin;
	}

private:
	DirectX::XMFLOAT3 rayPos;
	DirectX::XMFLOAT3 rayOrigin;
	float dx;
	float dy;
	float dz;
	float rayLength;
	float rayDistance;
	float rayLimit;
	float falloff;
};
