#include "WorldManager.h"

WorldManager::WorldManager(Graphics& gfx)
	: renderer(gfx)
{
}

void WorldManager::CreateChunk(int x, int y, int z)
{
	chunks.emplace(Position(x, y, z) , BasicChunk(x, y, z));
}

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		int i = 0;
		for (auto& block : chunk.second.blocks) {
			if(isVisible(chunk.second, block))
				renderer.Draw(block);
		}
		++i;
	}
}

int FlatIndexPure(int x, int y, int z) noexcept
{
	return x + 16 * (y + 16 * z);
}

bool WorldManager::isVisible(const BasicChunk& chunk, const Block& block)
{
	Position pos = chunk.Normalize(block.x, block.y, block.z);
	const std::vector<Block>& chunkData = chunk.blocks;
	
	if ((pos.x + 1 < BasicChunk::chunkSize) && (pos.x - 1 >= 0) &&
		(pos.y + 1 < BasicChunk::chunkSize) && (pos.y - 1 >= 0) &&
		(pos.z + 1 < BasicChunk::chunkSize) && (pos.z - 1 >= 0)) {

		if (chunkData[FlatIndexPure(pos.x + 1, pos.y, pos.z)].type != BlockType::Air
		&& chunkData[FlatIndexPure(pos.x - 1, pos.y, pos.z)].type != BlockType::Air
		&& chunkData[FlatIndexPure(pos.x, pos.y + 1, pos.z)].type != BlockType::Air
		&& chunkData[FlatIndexPure(pos.x, pos.y - 1, pos.z)].type != BlockType::Air
		&& chunkData[FlatIndexPure(pos.x, pos.y, pos.z + 1)].type != BlockType::Air
		&& chunkData[FlatIndexPure(pos.x, pos.y, pos.z - 1)].type != BlockType::Air) return false;
	}

	return true;
}
