#pragma once
#include "XM_Structs.h"

struct BillBoard {

	static constexpr float MeshLX = 0.4f;
	static constexpr float MeshLY = 0.5f;
	static constexpr float MeshLZ = MeshLX;
	static constexpr std::pair<std::array<Vertex, 16>, std::array<uint16_t, 24>> MeshL{
	{
		Vertex{DirectX::XMFLOAT3(MeshLX, MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(MeshLX, -MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, -MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(MeshLX, MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(MeshLX, -MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, -MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, -MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(MeshLX, MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(MeshLX, -MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshLX, -MeshLY, MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(MeshLX, MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(MeshLX, -MeshLY, -MeshLZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)}
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
	
	static constexpr float SaplingSide = 0.375f;
	static constexpr float SaplingSideY = SaplingSide;
	static constexpr std::pair<std::array<Vertex, 16>, std::array<uint16_t, 24>> Sapling{
	{
		Vertex{DirectX::XMFLOAT3(SaplingSide, SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, -SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, -SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, -SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(SaplingSide, -SaplingSide, -SaplingSide), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)}
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

	static constexpr float MeshSX = 0.4f;
	static constexpr float MeshSY = 0.5f;
	static constexpr float MeshSZ = MeshSX;
	static constexpr std::pair<std::array<Vertex, 16>, std::array<uint16_t, 24>> MeshS{
	{
		Vertex{DirectX::XMFLOAT3(MeshSX, MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(MeshSX, -MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, -MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(MeshSX, MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(MeshSX, -MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, -MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, -MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(MeshSX, MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(MeshSX, -MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 1)},
		Vertex{DirectX::XMFLOAT3(-MeshSX, -MeshSY, MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(2, 2)},
		Vertex{DirectX::XMFLOAT3(MeshSX, MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 1)},
		Vertex{DirectX::XMFLOAT3(MeshSX, -MeshSY, -MeshSZ), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1, 2)}
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
