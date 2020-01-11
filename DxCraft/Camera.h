#pragma once
#include "Graphics.h"
#include "ViewFrustum.h"

class Camera
{
public:
	Camera();
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Rotate(float dx, float dy) noexcept;
	DirectX::XMFLOAT3 Translate(DirectX::XMFLOAT3 translation, float travelSpeed = 12.0f, bool flying = false) noexcept;
	void SetPos(float x, float y, float z) noexcept;
	DirectX::XMFLOAT3 GetPos() noexcept;
	float GetPitch() noexcept;
	float GetYaw() noexcept;
	ViewFrustum GetFrustum() noexcept;
	static void UpdateProjection(float aspectRatio, float farZ);
	const DirectX::XMMATRIX& GetProjection();
private:
	DirectX::XMFLOAT3 pos{0.0f, 0.0f, 0.0f};
	static DirectX::XMMATRIX viewProjection;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	ViewFrustum frustum;
};