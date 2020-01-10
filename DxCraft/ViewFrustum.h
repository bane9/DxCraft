#pragma once
#include <DirectXMath.h>
#include "AABB.h"
#include <array>

struct Plane
{
	float DistanceToPoint(const DirectX::XMFLOAT3& point) const;

	float distanceToOrigin;
	DirectX::XMFLOAT3 normal;
};

class ViewFrustum
{
public:
	void Update(const DirectX::XMMATRIX& camMatrix) noexcept;

	bool IsBoxInFrustum(const AABB& box) const noexcept;

private:
	std::array<Plane, 6> planes;
};
