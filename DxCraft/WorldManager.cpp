#include "WorldManager.h"
#include "MathFunctions.h"

WorldManager::WorldManager(Graphics& gfx)
	: renderer(gfx)
{
}

void WorldManager::CreateChunk(int x, int y, int z)
{
	if (y < 0) return;
	chunks.emplace(Position(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize), 
		BasicChunk(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize));
}

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		for (auto& block : chunk.second.blocks) {
			if(isVisible(block))
				renderer.Draw(block);
		}
	}
}

Block* WorldManager::getBlock(int x, int y, int z)
{
	if (y < 0) 
		return nullptr;
	
	Position chunkPosition(
		x - FixedMod(x, BasicChunk::chunkSize),
		y - FixedMod(y, BasicChunk::chunkSize),
		z - FixedMod(z, BasicChunk::chunkSize)
	);

	auto chunk = chunks.find(chunkPosition);

	if (chunk == chunks.end()) 
		return nullptr;

	return &chunk->second.blocks[chunk->second.FlatIndex(x, y, z)];
}

#define TESTFACE(x) if (x == nullptr || x->type == BlockType::Air) return true

bool WorldManager::isVisible(const Block& block)
{
	TESTFACE(getBlock(block.x + 1, block.y, block.z));
	TESTFACE(getBlock(block.x - 1, block.y, block.z));
	TESTFACE(getBlock(block.x, block.y + 1, block.z));
	TESTFACE(getBlock(block.x, block.y - 1, block.z));
	TESTFACE(getBlock(block.x, block.y, block.z + 1));
	TESTFACE(getBlock(block.x, block.y, block.z - 1));

	return false;
}
