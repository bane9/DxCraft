#pragma once
#include <DirectXMath.h>

class AABB
{
public:
	AABB(const DirectX::XMFLOAT3& dimension);
	void SetPosition(const DirectX::XMFLOAT3& dimension) noexcept;
	DirectX::XMFLOAT3 GetVN(const DirectX::XMFLOAT3& normal)  const noexcept;
	DirectX::XMFLOAT3 GetVP(const DirectX::XMFLOAT3& normal)  const noexcept;
	static bool IsColided(const AABB& obj1, const AABB& obj2) noexcept;
	DirectX::XMFLOAT3 dimension;
	DirectX::XMFLOAT3 position;
	static float collision_x_depth(const AABB& obj1, const AABB& obj2, float eps = 0.00128f) noexcept;
	static float collision_y_depth(const AABB& obj1, const AABB& obj2, float eps = 0.00128f) noexcept;
	static float collision_z_depth(const AABB& obj1, const AABB& obj2, float eps = 0.00128f) noexcept;
	bool IsPointInside(DirectX::XMFLOAT3 point);
};
