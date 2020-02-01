#pragma once
#include <memory>
#include "Block.h"
#include "Chunk.h"
#include "Graphics.h"
#include <vector>
#include "XM_Structs.h"
#include "robin_hood.h"
#include "Camera.h"
#include "RenderData.h"
#include <DirectXMath.h>
#include <algorithm>
#include <type_traits>
#include <map>
#include "WorldGenerator.h"

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z, bool empty = false);
	bool ModifyBlock(int x, int y, int z, Block::BlockType type = Block::BlockType::Air);
	bool ModifyBlock(const Position& pos, Block::BlockType type = Block::BlockType::Air);
	void RenderChunks(Camera& cam);
	void UnloadChunks(const Position& pos, float area = 20.0f);
	std::shared_ptr<Block> GetBlock(int x, int y, int z, bool safetyCheck = true);
	std::shared_ptr<Block> GetBlock(const Position& pos, bool safetyCheck = true);
	std::shared_ptr<Block> GetBlock(const DirectX::XMFLOAT3& pos, bool safetyCheck = true);
	std::shared_ptr<Chunk> GetChunkFromBlock(int x, int y, int z, bool safetyCheck = true);
	bool CreateChunkAtPlayerPos(const Position& pos);
public:
	void GenerateMesh(std::shared_ptr<Chunk> chunkPtr);
	robin_hood::unordered_flat_map <Position, std::shared_ptr<Chunk>, PositionHash> chunks;
	Graphics& gfx;
	WorldGenerator wGen;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	RenderData renderData;
	bool lockThread = false;
	bool creatingChunks = false;
};
