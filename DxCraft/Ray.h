#pragma once
#include <DirectXMath.h>
#include "BasicChunk.h"
#include "imgui/imgui.h"
#include <math.h>
#include "WorldManager.h"
#include "Block.h"
#include "MathFunctions.h"

class Ray {
public:
	Ray(WorldManager& wManager, float limit = 8.0f)
		: wManager(wManager), rayLimit(limit)
	{}

	void SetPositionAndDirection(const DirectX::XMFLOAT3& startCoord, float pitch, float yaw) noexcept {
		rayPos = startCoord;
		rayPos.x += 0.5f * sgn(rayPos.x);
		rayPos.y += 0.5f * sgn(rayPos.y);
		rayPos.z += 0.5f * sgn(rayPos.z);

		dx = std::sin(yaw) * std::cos(pitch);
		dy = std::sin(-pitch);
		dz = std::cos(yaw) * std::cos(pitch);
		
		rayDistance = 0.0f;
	}

	Position GetBlock(float precision = 0.01f) noexcept {
		Position pos(-1, -1, -1);

		while(rayDistance < rayLimit) {
			rayPos.x += dx * precision;
			rayPos.y += dy * precision;
			rayPos.z += dz * precision;
			auto block = wManager.GetBlock(rayPos.x, rayPos.y, rayPos.z);
			
			if (block != nullptr && block->type != BlockType::Air) {
				pos.x = block->x;
				pos.y = block->y;
				pos.z = block->z;
				break;
			}
				
			rayDistance += precision;
		}
		return pos;
	}

private:
	DirectX::XMFLOAT3 rayPos;
	WorldManager& wManager;
	float dx;
	float dy;
	float dz;
	float rayLength;
	float rayDistance = 0.0f;
	float rayLimit;
};
