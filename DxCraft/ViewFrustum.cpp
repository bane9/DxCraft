#include "ViewFrustum.h"
#include "MathFunctions.h"


enum Planes
{
	Near,
	Far,
	Left,
	Right,
	Top,
	Bottom
};

float Plane::DistanceToPoint(const DirectX::XMFLOAT3& point) const
{
	return Dot3D(point, normal) + distanceToOrigin;
}

void ViewFrustum::Update(const DirectX::XMMATRIX& camMatrix) noexcept
{
	DirectX::XMFLOAT4X4 mat;
	DirectX::XMStoreFloat4x4(&mat, camMatrix * DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 100.0f));

	planes[Planes::Left].normal.x = mat._14 + mat._11;
	planes[Planes::Left].normal.y = mat._24 + mat._21;
	planes[Planes::Left].normal.z = mat._34 + mat._31;
	planes[Planes::Left].distanceToOrigin = mat._44 + mat._41;

	planes[Planes::Right].normal.x = mat._14 - mat._11;
	planes[Planes::Right].normal.y = mat._24 - mat._21;
	planes[Planes::Right].normal.z = mat._34 - mat._31;
	planes[Planes::Right].distanceToOrigin = mat._44 - mat._41;

	planes[Planes::Bottom].normal.x = mat._14 + mat._12;
	planes[Planes::Bottom].normal.y = mat._24 + mat._22;
	planes[Planes::Bottom].normal.z = mat._34 + mat._32;
	planes[Planes::Bottom].distanceToOrigin = mat._44 + mat._42;

	planes[Planes::Top].normal.x = mat._14 - mat._12;
	planes[Planes::Top].normal.y = mat._24 - mat._22;
	planes[Planes::Top].normal.z = mat._34 - mat._32;
	planes[Planes::Top].distanceToOrigin = mat._44 - mat._42;

	planes[Planes::Near].normal.x = mat._14 + mat._13;
	planes[Planes::Near].normal.y = mat._24 + mat._23;
	planes[Planes::Near].normal.z = mat._34 + mat._33;
	planes[Planes::Near].distanceToOrigin = mat._44 + mat._43;

	planes[Planes::Far].normal.x = mat._14 - mat._13;
	planes[Planes::Far].normal.y = mat._24 - mat._23;
	planes[Planes::Far].normal.z = mat._34 - mat._33;
	planes[Planes::Far].distanceToOrigin = mat._44 - mat._43;

	for (auto& plane : planes)
	{
		float length = VectorLength(plane.normal);
		plane.normal.x /= length;
		plane.normal.y /= length;
		plane.normal.z /= length;
		plane.distanceToOrigin /= length;
	}

}

bool ViewFrustum::IsBoxInFrustum(const AABB& box) const noexcept
{
	bool result = true;
	for (auto& plane : planes)
	{
		if (plane.DistanceToPoint(box.GetVP(plane.normal)) < 0)
		{
			return false;
		}
		else if (plane.DistanceToPoint(box.GetVN(plane.normal)) < 0)
		{
			result = true;
		}
	}
	return result;
}
