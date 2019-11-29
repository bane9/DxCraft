#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "Graphics.h"
#include <vector>
#include "XM_Structs.h"
#include "robin_hood.h"
#include "Camera.h"

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z, bool empty = false);
	void ModifyBlock(int x, int y, int z, BlockType type = BlockType::Air);
	void GenerateMeshes();
	void Draw(Graphics& gfx, Camera& cam);
	Block* GetBlock(int x, int y, int z);
private:
	void GenerateMesh(BasicChunk& chunk);
	void AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, 
		BasicChunk& chunk, const std::array<float, 2>& texture, float offsetX, float offsetY, float offsetZ);
	bool BlockVisible(const BasicChunk& chunk, int x, int y, int z);
	BasicChunk* GetChunkFromBlock(int x, int y, int z);
	robin_hood::unordered_flat_map <Position, BasicChunk, PositionHash> chunks;
	
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	//RendererData<Vertex, Transforms> chunkRenderData;
};