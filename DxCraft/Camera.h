#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Rotate(float dx, float dy) noexcept;
	DirectX::XMFLOAT3 Translate(DirectX::XMFLOAT3 translation, float travelSpeed = 12.0f, 
		bool flying = false, bool midAir = false) noexcept;
	void SetPos(float x, float y, float z) noexcept;
	DirectX::XMFLOAT3 GetPos() noexcept;
	float GetPitch() noexcept;
	float GetYaw() noexcept;
private:
	DirectX::XMFLOAT3 pos{0.0f, 0.0f, 0.0f};
	float pitch = 0.0f;
	float yaw = 0.0f;
	float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};