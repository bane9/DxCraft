#include "Camera.h"
#include "MathFunctions.h"
#include <algorithm>

Camera::Camera() noexcept
{
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	const DirectX::XMVECTOR camPosition = DirectX::XMLoadFloat3(&pos);
	const DirectX::XMVECTOR camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SetPos(float x, float y, float z) noexcept {
	pos = { x, y, z };
}

DirectX::XMFLOAT3 Camera::GetPos() noexcept
{
	return pos;
}

float Camera::GetPitch() noexcept
{
	return pitch;
}

float Camera::GetYaw() noexcept
{
	return yaw;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation, float travelSpeed) noexcept
{
	if (translation.y == 0) {
		DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));
	}
	else {
		DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));
	}

	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}

