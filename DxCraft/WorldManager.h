#pragma once
#include "Block.h"
#include "BasicChunk.h"
#include "BlockRenderer.h"
#include "Graphics.h"
#include <vector>

class WorldManager
{
public:
	WorldManager(Graphics& gfx);
	WorldManager(WorldManager&) = delete;
	WorldManager& operator=(WorldManager&) = delete;
	void CreateChunk(int x, int y, int z);
	void Draw();
private:
	std::vector<BasicChunk> chunks;
	BlockRenderer renderer;
};

