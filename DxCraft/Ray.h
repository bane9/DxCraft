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
		rayPos = startCoord;

		dx = std::sin(yaw) * std::cos(pitch);
		dy = std::sin(-pitch);
		dz = std::cos(yaw) * std::cos(pitch);
		
		rayDistance = 0.0f;

		falloff = 0.75f;
	}

	bool Next() noexcept
	{
		rayPos.x += dx * falloff;
		rayPos.y += dy * falloff;
		rayPos.z += dz * falloff;
		rayDistance += falloff;
		falloff *= 0.75f;
		falloff = std::clamp(falloff, 0.05f, 0.75f);
		return rayDistance < rayLimit;
	}


	DirectX::XMFLOAT3 GetVector() const noexcept 
	{
		return rayPos;
	}

private:
	DirectX::XMFLOAT3 rayPos;
	float dx;
	float dy;
	float dz;
	float rayLength;
	float rayDistance;
	float rayLimit;
	float falloff;
};
