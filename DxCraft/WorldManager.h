#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "Graphics.h"
#include <vector>
#include "XM_Structs.h"
#include "robin_hood.h"
#include "Camera.h"
#include "RenderData.h"
#include <DirectXMath.h>
#include <algorithm>
#include <type_traits>

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	BasicChunk* CreateChunk(int x, int y, int z, bool empty = false);
	bool ModifyBlock(int x, int y, int z, Block::BlockType type = Block::BlockType::Air);
	bool ModifyBlock(const Position& pos, Block::BlockType type = Block::BlockType::Air);
	void GenerateMeshes();
	void RenderChunks(Camera& cam);
	Block* GetBlock(int x, int y, int z);
	Block* GetBlock(const Position& pos);
	Block* GetBlock(const DirectX::XMFLOAT3& pos);
	BasicChunk* GetChunkFromBlock(int x, int y, int z);
	BasicChunk* CreateChunkAtPlayerPos(const Position& pos);
public:
	void GenerateMesh(BasicChunk& chunk);
	template<typename Container, typename UVs>
	void AppendMesh(const Container& mesh,
		std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
		const UVs& textures, float offsetX, float offsetY, float offsetZ);
	bool BlockVisible(const BasicChunk& chunk, int x, int y, int z, Block::BlockType type = Block::BlockType::None);
	robin_hood::unordered_flat_map <Position, BasicChunk, PositionHash> chunks;
	Graphics& gfx;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	RenderData renderData;
};

template <class>
inline constexpr bool is_array_of_array_v = false;

template <class T, std::size_t N, std::size_t M>
inline constexpr bool is_array_of_array_v<std::array<std::array<T, N>, M>> = true;

template<typename Container, typename UVs>
inline void WorldManager::AppendMesh(const Container& mesh,
	std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
	const UVs& textures, float offsetX, float offsetY, float offsetZ)
{
	if constexpr (is_array_of_array_v<UVs>) {
		for (int i = 0; i < textures.size(); i++) {
			std::transform(mesh.first.begin(), mesh.first.end(), std::back_inserter(vertexBuffer),
				[offsetX, offsetY, offsetZ, &textures, i](Vertex vertex) {
					vertex.pos.x += offsetX;
					vertex.pos.y += offsetY;
					vertex.pos.z += offsetZ;
					float startU, endU, startV, endV;
					startU = textures[i][0] / 16.0f;
					endU = (textures[i][0] + 1.0f) / 16.0f;
					startV = textures[i][1] / 16.0f;
					endV = (textures[i][1] + 1.0f) / 16.0f;
					vertex.tc.x = vertex.tc.x == 1 ? startU : endU;
					vertex.tc.y = vertex.tc.y == 1 ? startV : endV;
					return vertex;
				});
		}
	}
	else {
		std::transform(mesh.first.begin(), mesh.first.end(), std::back_inserter(vertexBuffer),
			[offsetX, offsetY, offsetZ, &textures](Vertex vertex) {
				vertex.pos.x += offsetX;
				vertex.pos.y += offsetY;
				vertex.pos.z += offsetZ;
				float startU, endU, startV, endV;
				startU = textures[0] / 16.0f;
				endU = (textures[0] + 1.0f) / 16.0f;
				startV = textures[1] / 16.0f;
				endV = (textures[1] + 1.0f) / 16.0f;
				vertex.tc.x = vertex.tc.x == 1 ? startU : endU;
				vertex.tc.y = vertex.tc.y == 1 ? startV : endV;
				return vertex;
			});
	}
	const int offset = vertexBuffer.size() > 0 ? (vertexBuffer.size() / mesh.first.size() - 1) * mesh.first.size() : 0;
	std::transform(mesh.second.begin(), mesh.second.end(), std::back_inserter(indexBuffer), 
		[offset](int a) {return offset + a;});
}
