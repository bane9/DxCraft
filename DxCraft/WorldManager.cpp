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
		int i = 0;
		for (auto& block : chunk.blocks) {
			if(isVisible(i, block))
				renderer.Draw(block);
		}
		++i;
	}
}

int FlatIndexPure(int x, int y, int z) noexcept
{
	return x + 16 * (y + 16 * z);
}

bool WorldManager::isVisible(int chunkIndex, Block& block)
{
	BasicChunk& chunk = chunks[chunkIndex];
	Position pos = chunk.Normalize(block.x, block.y, block.z);
	std::vector<Block>& chunkData = chunk.blocks;
	int nonVisibleFaces = 0;

	if (pos.x + 1 < BasicChunk::chunkSize) {
		if (chunkData[FlatIndexPure(pos.x + 1, pos.y, pos.z)].type != BlockType::Air) ++nonVisibleFaces;
	}

	if (pos.x - 1 >= 0) {
		if (chunkData[FlatIndexPure(pos.x - 1, pos.y, pos.z)].type != BlockType::Air) ++nonVisibleFaces;
	}

	if (pos.y + 1 < BasicChunk::chunkSize) {
		if (chunkData[FlatIndexPure(pos.x, pos.y + 1, pos.z)].type != BlockType::Air) ++nonVisibleFaces;
	}

	if (pos.y - 1 >= 0) {
		if (chunkData[FlatIndexPure(pos.x, pos.y - 1, pos.z)].type != BlockType::Air) ++nonVisibleFaces;
	}

	if (pos.z + 1 < BasicChunk::chunkSize) {
		if (chunkData[FlatIndexPure(pos.x, pos.y, pos.z + 1)].type != BlockType::Air) ++nonVisibleFaces;
	}

	if (pos.z - 1 >= 0) {
		if (chunkData[FlatIndexPure(pos.x, pos.y, pos.z - 1)].type != BlockType::Air) ++nonVisibleFaces;
	}

	return nonVisibleFaces != 6;
}
