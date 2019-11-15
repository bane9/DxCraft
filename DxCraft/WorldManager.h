#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "BlockRenderer.h"
#include "Graphics.h"
#include <vector>
#include <unordered_map>

auto asd = []() {return true; };

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z);
	void Draw();
private:
	bool isVisible(const BasicChunk& chunk, const Block& block);
	std::unordered_map < Position, BasicChunk, PositionHash, PositionComparator > chunks;
	BlockRenderer renderer;
};