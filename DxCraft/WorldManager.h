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

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z, bool empty = false);
	void ModifyBlock(int x, int y, int z, Block::BlockType type = Block::BlockType::Air);
	void GenerateMeshes();
	void DrawOpaque(Camera& cam);
	void DrawTransparent(Camera& cam);
	Block* GetBlock(int x, int y, int z);
	Block* GetBlock(const DirectX::XMFLOAT3& pos);
private:
	void GenerateMesh(BasicChunk& chunk);
	void AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face,
		std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
		const std::array<float, 2>& texture, float offsetX, float offsetY, float offsetZ);
	template<typename Container>
	void AppendFullMesh(const Container& mesh,
		std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
		const std::array<std::array<float, 2>, 6>& textures, float offsetX, float offsetY, float offsetZ);
	bool BlockVisible(const BasicChunk& chunk, int x, int y, int z, Block::BlockType type = Block::BlockType::None);
	BasicChunk* GetChunkFromBlock(int x, int y, int z);
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

template<typename Container>
inline void WorldManager::AppendFullMesh(const Container& mesh,
	std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
	const std::array<std::array<float, 2>, 6>& textures, float offsetX, float offsetY, float offsetZ)
{
	for (int i = 0; i < textures.size(); i++) {
		std::transform(mesh.first.begin(), mesh.first.end(), std::back_inserter(vertexBuffer),
			[offsetX, offsetY, offsetZ, &textures, i](Vertex vertex) {

				vertex.pos.x += offsetX;
				vertex.pos.y += offsetY;
				vertex.pos.z += offsetZ;
				const float startTexX = textures[i][0] / 16.0f;
				const float endTexX = (textures[i][0] + 1.0f) / 16.0f;
				const float startTexY = textures[i][1] / 16.0f;
				const float endTexY = (textures[i][1] + 1.0f) / 16.0f;
				vertex.tc.x = vertex.tc.x == 1 ? startTexX : endTexX;
				vertex.tc.y = vertex.tc.y == 1 ? startTexY : endTexY;
				return vertex;

			});
	}
	const int offset = vertexBuffer.size() > 0 ? (vertexBuffer.size() / mesh.first.size() - 1) * mesh.first.size() : 0;
	std::transform(mesh.second.begin(), mesh.second.end(), std::back_inserter(indexBuffer), 
		[offset](int a) {return offset + a; });
	
}
