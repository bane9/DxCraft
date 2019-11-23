#pragma once
#include "XM_Structs.h"

struct BlockSelector {
	static constexpr float side = 0.5001f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 36>> Cube{
		{
			DirectX::XMFLOAT3(-side,-side,-side),
			DirectX::XMFLOAT3(side,-side,-side),
			DirectX::XMFLOAT3(-side,side,-side),
			DirectX::XMFLOAT3(side,side,-side),
			DirectX::XMFLOAT3(-side,-side,side),
			DirectX::XMFLOAT3(side,-side,side),
			DirectX::XMFLOAT3(-side,side,side),
			DirectX::XMFLOAT3(side,side,side)
		},
		{
			0, 2, 1, 2, 3, 1,
			1, 3, 5, 3, 7, 5,
			2, 6, 3, 3, 6, 7,
			4, 5, 7, 4, 7, 6,
			0, 4, 2, 2, 4, 6,
			0, 1, 4, 1, 5, 4
		}
	};
};
