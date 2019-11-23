#pragma once
#include <DirectXMath.h>
#include <array>

struct Crosshair {
	static constexpr float side = 0.01f;
	static constexpr float z_offset = 0.51f;
	static constexpr float scale = 0.1f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 12>> NearSide{
		{
		DirectX::XMFLOAT3(-0.1f  * scale, -0.01f * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(0.1f   * scale, -0.01f * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(-0.1f  * scale,  0.01f * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(0.1f   * scale,  0.01f * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(0.01f  * scale, -0.1f  * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(0.01f  * scale,  0.1f  * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(-0.01f * scale, -0.1f  * scale, 0.0f + z_offset),
		DirectX::XMFLOAT3(-0.01f * scale,  0.1f  * scale, 0.0f + z_offset)
		},
		{
			1, 0, 2,
			5, 4, 6,
			1, 2, 3,
			5, 6, 7
		}
	};
};
