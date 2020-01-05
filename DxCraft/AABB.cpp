#include "AABB.h"
#include "MathFunctions.h"

AABB::AABB(const DirectX::XMFLOAT3& dimension)
	: dimension(dimension)
{
}

void AABB::SetPosition(const DirectX::XMFLOAT3& position) noexcept
{
	this->position = position;
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
	return (obj1.position.x <= obj2.position.x + obj2.dimension.x && obj1.position.x + obj1.dimension.x >= obj2.position.x) &&
		(obj1.position.y <= obj2.position.y + obj2.dimension.y && obj1.position.y + obj1.dimension.y >= obj2.position.y) &&
		(obj1.position.z <= obj2.position.z + obj2.dimension.z && obj1.position.z + obj1.dimension.z >= obj2.position.z);
}

float AABB::collision_x_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.x + obj1.dimension.x > obj2.position.x + obj2.dimension.x)
		return (obj2.position.x + obj2.dimension.x) - obj1.position.x + eps;
	else
		return obj2.position.x - (obj1.position.x + obj1.dimension.x) - eps;
}

float AABB::collision_y_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.y + obj1.dimension.y > obj2.position.y + obj2.dimension.y)
		return (obj2.position.y + obj2.dimension.y) - obj1.position.y + eps;
	else
		return obj2.position.y - (obj1.position.y + obj1.dimension.y) - eps;
}

float AABB::collision_z_depth(const AABB& obj1, const AABB& obj2, float eps) noexcept
{
	if (obj1.position.z + obj1.dimension.z > obj2.position.z + obj2.dimension.z)
		return (obj2.position.z + obj2.dimension.z) - obj1.position.z + eps;
	else
		return obj2.position.z - (obj1.position.z + obj1.dimension.z) - eps;
}

bool AABB::IsLineIntersected(DirectX::XMFLOAT3 orig, DirectX::XMFLOAT3 dest)
{
	const float min_x = position.x;
	const float min_y = position.y;
	const float min_z = position.z;

	const float max_x = min_x + dimension.x;
	const float max_y = min_y + dimension.y;
	const float max_z = min_z + dimension.z;

	float tmin = (min_x - orig.x) / dest.x;
	float tmax = (max_x - orig.x) / dest.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (min_y - orig.y) / dest.y;
	float tymax = (max_y - orig.y) / dest.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmax = (max_z - orig.z) / dest.z;
	float tzmin = (min_z - orig.z) / dest.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	return true;
}

bool AABB::IsPointInside(const DirectX::XMFLOAT3& point)
{
	const float min_x = position.x - dimension.x * 0.5f;
	const float min_y = position.y - dimension.y * 0.5f;
	const float min_z = position.z - dimension.z * 0.5f;

	const float max_x = position.x + dimension.x * 0.5f;
	const float max_y = position.y + dimension.y * 0.5f;
	const float max_z = position.z + dimension.z * 0.5f;

	const float x = point.x;
	const float y = point.y;
	const float z = point.z;

	return min_x <= x && x <= max_x && min_y <= y && y <= max_y && min_z <= z && z <= max_z;
}

