#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "BlockRenderer.h"
#include "Graphics.h"
#include <vector>
#include <unordered_map>

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z);
	void Draw();
private:
	Block* getBlock(int x, int y, int z);
	bool isVisible(const Block& block);
	std::unordered_map <Position, BasicChunk, PositionHash> chunks;
	BlockRenderer renderer;
};