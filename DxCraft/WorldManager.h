#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "MeshRenderer.h"
#include "Graphics.h"
#include <vector>
#include <unordered_map>
#include "XM_Structs.h"

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z);
	void Draw();
private:
	void GenerateMesh(const BasicChunk& chunk);
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	void AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, float offsetX, float offsetY, float offsetZ);
	Block* getBlock(int x, int y, int z);
	bool isVisible(const Block& block);
	std::unordered_map <Position, BasicChunk, PositionHash> chunks;
	MeshRenderer renderer;
};