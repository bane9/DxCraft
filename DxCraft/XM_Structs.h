#pragma once
#include <DirectXmath.h>
#include <array>
#include <utility>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tc;
	float light;
};

struct Transforms
{
	DirectX::XMMATRIX modelViewProj;
	DirectX::XMMATRIX model;
};

static constexpr float sideLight = 0.85f;
static constexpr float bottomLight = 0.75f;

struct Faces {
	static constexpr float side = 0.499999f;
	
	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> NearSide{
		{
		Vertex{DirectX::XMFLOAT3(-side ,-side,-side), { 1, 2 },  sideLight},
		Vertex{DirectX::XMFLOAT3(side ,-side,-side),  { 2, 2 },  sideLight},
		Vertex{DirectX::XMFLOAT3(-side ,side,-side),  { 1, 1 },  sideLight},
		Vertex{DirectX::XMFLOAT3(side ,side,-side),	  { 2, 1 }, sideLight}
		},
		{0, 2, 1, 2, 3, 1}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> FarSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,side),  { 1, 2 }, sideLight},
		Vertex{DirectX::XMFLOAT3(side,-side,side), { 2, 2 }, sideLight},
		Vertex{DirectX::XMFLOAT3(-side,side,side),  { 1, 1 }, sideLight},
		Vertex{DirectX::XMFLOAT3(side,side,side) , { 2, 1 } , sideLight}
		},
		{0, 1, 3, 0, 3, 2}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> LeftSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,-side), { 1, 2 } , sideLight},
		Vertex{DirectX::XMFLOAT3(-side,side,-side),  { 1, 1 } , sideLight},
		Vertex{DirectX::XMFLOAT3(-side,-side,side),  { 2, 2 } , sideLight},
		Vertex{DirectX::XMFLOAT3(-side,side,side),	 { 2, 1 }, sideLight}
		},
		{0, 2, 1, 2, 3, 1}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> RightSide{
		{
		Vertex{DirectX::XMFLOAT3(side,-side,-side), { 1, 2 }, sideLight},
		Vertex{DirectX::XMFLOAT3(side,side,-side),  { 1, 1 }, sideLight},
		Vertex{DirectX::XMFLOAT3(side,-side,side),  { 2, 2 }, sideLight},
		Vertex{DirectX::XMFLOAT3(side,side,side), { 2, 1 }  , sideLight}
		},
		{0, 1, 3, 0, 3, 2}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> BottomSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,-side), { 1, 2 }, bottomLight},
		Vertex{DirectX::XMFLOAT3(side,-side,-side), { 2, 2 } , bottomLight},
		Vertex{DirectX::XMFLOAT3(-side,-side,side), { 1, 1 } , bottomLight},
		Vertex{DirectX::XMFLOAT3(side,-side,side), { 2, 1 }  , bottomLight}
		},
		{0, 1, 2, 2, 1, 3}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> TopSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,side,-side), { 1, 2 } , 1.0f},
		Vertex{DirectX::XMFLOAT3(side,side,-side),  { 2, 2 } , 1.0f},
		Vertex{DirectX::XMFLOAT3(-side,side,side),  { 1, 1 } , 1.0f},
		Vertex{DirectX::XMFLOAT3(side,side,side),	{ 2, 1 }, 1.0f}
		},
		{0, 3, 1, 0, 2, 3}
	};
};
