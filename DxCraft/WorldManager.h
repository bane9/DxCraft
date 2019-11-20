#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "MeshRenderer.h"
#include "Graphics.h"
#include <vector>
#include "XM_Structs.h"
#include <unordered_map>

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z);
	void ModifyBlock(int x, int y, int z, BlockType type = BlockType::Air);
	void GenerateMeshes();
	void Draw();
	Block* GetBlock(int x, int y, int z);
private:
	void GenerateMesh(BasicChunk& chunk);
	void AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, 
		BasicChunk& chunk, const std::array<float, 2>& texture, float offsetX, float offsetY, float offsetZ);
	bool BlockVisible(const BasicChunk& chunk, int x, int y, int z);
	BasicChunk* GetChunkFromBlock(int x, int y, int z);
	std::unordered_map <Position, BasicChunk, PositionHash> chunks;
	MeshRenderer renderer;
};