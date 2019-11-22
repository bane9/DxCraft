#pragma once
#include <DirectXMath.h>
#include <array>

struct CrosshairVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tc;
};

struct Crosshair {
	static constexpr float side = 0.01f;
	static constexpr float z_offset = 0.1f;
	static constexpr std::pair<std::array<CrosshairVertex, 4>, std::array<uint16_t, 6>> NearSide{
		{
		CrosshairVertex{DirectX::XMFLOAT3(-side,-side, -side + z_offset), { 1, 2 }},
		CrosshairVertex{DirectX::XMFLOAT3(side, -side, -side + z_offset),  { 2, 2 }},
		CrosshairVertex{DirectX::XMFLOAT3(-side, side, -side + z_offset),  { 1, 1 }},
		CrosshairVertex{DirectX::XMFLOAT3(side,  side, -side + z_offset),  { 2, 1 }}
		},
		{0, 2, 1, 2, 3, 1}
	};
};
