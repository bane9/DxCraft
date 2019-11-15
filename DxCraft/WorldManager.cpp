#include "WorldManager.h"

WorldManager::WorldManager(Graphics& gfx)
	: renderer(gfx)
{
}

void WorldManager::CreateChunk(int x, int y, int z)
{
	chunks.push_back({x, y, z});
}

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		for (auto& block : chunk.blocks) {
			renderer.Draw(block);
		}
	}
}
