#pragma once
#include "XM_Structs.h"

struct BillBoard {
	static constexpr std::pair<std::array<Vertex, 16>, std::array<uint16_t, 24>> Mesh{
	{
		Vertex{DirectX::XMFLOAT3(0.4, 0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, -0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, 0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, -0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 5.0f / 16.0f)},//--
		Vertex{DirectX::XMFLOAT3(0.4, 0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, -0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, 0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, -0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, 0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, -0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, 0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, -0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(-0.4, 0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 4.0f / 16.0f)},//
		Vertex{DirectX::XMFLOAT3(-0.4, -0.5, 0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(10.0f / 16.0f, 5.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, 0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 4.0f / 16.0f)},
		Vertex{DirectX::XMFLOAT3(0.4, -0.5, -0.4), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(9.0f / 16.0f, 5.0f / 16.0f)}
	},
	{
		1, 2, 0,
		5, 6, 4,
		9, 10, 8,
		13, 14, 12,
		1, 3, 2,
		5, 7, 6,
		9, 11, 10,
		13, 15, 14,
	}
	};
};
