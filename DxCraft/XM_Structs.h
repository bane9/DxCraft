#pragma once
#include <DirectXmath.h>
#include <array>
#include <utility>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 n;
	DirectX::XMFLOAT2 tc;
};

struct Transforms
{
	DirectX::XMMATRIX modelViewProj;
	DirectX::XMMATRIX model;
};

struct Faces {
	static constexpr float side = 1.0f;
	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> NearSide{
		{
		Vertex{DirectX::XMFLOAT3(-side ,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side ,-side,-side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side ,side,-side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(side ,side,-side),	  DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 2, 1, 2, 3, 1}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> FarSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side,-side,side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side,side,side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(side,side,side) ,	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 3, 0, 3, 2}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> LeftSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side,side,-side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 2, 1, 2, 3, 1}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> RightSide{
		{
		Vertex{DirectX::XMFLOAT3(side,-side,-side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side,side,-side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(side,-side,side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 3, 0, 3, 2}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> BottomSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,-side,-side), DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side,-side,-side),  DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side,-side,side),  DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(side,-side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 1, 2, 2, 1, 3}
	};

	static constexpr std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>> TopSide{
		{
		Vertex{DirectX::XMFLOAT3(-side,side,-side),  DirectX::XMFLOAT3(), { 0.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(side,side,-side),   DirectX::XMFLOAT3(), { 1.0f,0.0f }},
		Vertex{DirectX::XMFLOAT3(-side,side,side),   DirectX::XMFLOAT3(), { 0.0f,1.0f }},
		Vertex{DirectX::XMFLOAT3(side,side,side),	 DirectX::XMFLOAT3(), { 1.0f,1.0f }}
		},
		{0, 3, 1, 0, 2, 3}
	};
};
