#include "AABB.h"

AABB::AABB(const DirectX::XMFLOAT3& dimension)
	: dimension(dimension)
{
}

void AABB::Update(const DirectX::XMFLOAT3& position) noexcept
{
	this->position = position;
}

float  AABB::hitbox_x_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.x + obj1.dimension.x > obj2.position.x + obj2.dimension.x)
	{
		return (obj2.position.x + obj2.dimension.x) - obj1.position.x + eps;
	}
	else
	{
		return obj2.position.x - (obj1.position.x + obj1.dimension.x) - eps;
	}
}

float AABB::hitbox_y_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.y + obj1.dimension.y > obj2.position.y + obj2.dimension.y)
	{
		return (obj2.position.y + obj2.dimension.y) - obj1.position.y + eps;
	}
	else
	{
		return obj2.position.y - (obj1.position.y + obj1.dimension.y) - eps;
	}
}

float AABB::hitbox_z_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.z + obj1.dimension.z > obj2.position.z + obj2.dimension.z)
	{
		return (obj2.position.z + obj2.dimension.z) - obj1.position.z + eps;
	}
	else
	{
		return obj2.position.z - (obj1.position.z + obj1.dimension.z) - eps;
	}
}


DirectX::XMFLOAT3 AABB::GetVN(const DirectX::XMFLOAT3& normal) const noexcept
{
	DirectX::XMFLOAT3 res = position;

	if (normal.x < 0)
		res.x += dimension.x;

	if (normal.y < 0)
		res.y += dimension.y;

	if (normal.z < 0)
		res.z += dimension.z;

	return res;
}

DirectX::XMFLOAT3 AABB::GetVP(const DirectX::XMFLOAT3& normal) const noexcept
{
	DirectX::XMFLOAT3 res = position;

	if (normal.x > 0)
		res.x += dimension.x;

	if (normal.y > 0)
		res.y += dimension.y;

	if (normal.z > 0)
		res.z += dimension.z;

	return res;
}

bool AABB::IsColided(const AABB& obj1, const AABB& obj2) noexcept
{
	return (obj1.position.x <= obj2.position.x + obj2.dimension.x && obj1.position.x + obj1.dimension.x >= obj2.position.x) ||
		(obj1.position.y <= obj2.position.y + obj2.dimension.y && obj1.position.y + obj1.dimension.y >= obj2.position.y) ||
		(obj1.position.z <= obj2.position.z + obj2.dimension.z && obj1.position.z + obj1.dimension.z >= obj2.position.z);
}
