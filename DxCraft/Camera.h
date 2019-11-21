#pragma once
#include "Graphics.h"

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void setTravelSpeed(float speed);
	DirectX::XMFLOAT3 getLookVector();
	float getTravelSpeed();
	void SetPos(float x, float y, float z) noexcept;
	DirectX::XMFLOAT3 GetPos();
	float GetPitch();
	float GetYaw();
private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};