#pragma once
#include <DirectXMath.h>
#include <math.h>
#include "MathFunctions.h"

class Ray {
public:
	Ray(WorldManager& wManager, float limit = 8.0f)
		: wManager(wManager), rayLimit(limit)
	{}

	void SetPositionAndDirection(const DirectX::XMFLOAT3& startCoord, float pitch, float yaw, float startDisance = 0.0f) noexcept {
		rayPos = startCoord;

		dx = std::sin(yaw) * std::cos(pitch);
		dy = std::sin(-pitch);
		dz = std::cos(yaw) * std::cos(pitch);
		
		rayDistance = 0.0f;

		while(rayDistance < startDisance)
			Next(0.5f);
	}

	bool Next(float precision = 0.05f) noexcept
	{
		rayPos.x += dx * precision;
		rayPos.y += dy * precision;
		rayPos.z += dz * precision;
		rayDistance += precision;
		return rayDistance < rayLimit;
	}


	DirectX::XMFLOAT3 GetVector() {
		return rayPos;
	}

private:
	bool once = false;
	DirectX::XMFLOAT3 rayPos;
	WorldManager& wManager;
	float dx;
	float dy;
	float dz;
	float rayLength;
	float rayDistance = 0.0f;
	float rayLimit;
};
