#include "WorldManager.h"
#include "MathFunctions.h"

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
			if(isVisible(block))
				renderer.Draw(block);
		}
		++i;
	}
}

Block* WorldManager::getBlock(int x, int y, int z)
{
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

bool WorldManager::isVisible(const Block& block)
{
	Block* north = getBlock(block.x, block.y + 1, block.z);
	if (north == nullptr || north->type == BlockType::Air) return true;
	Block* south = getBlock(block.x, block.y - 1, block.z + 1);
	if (south == nullptr || south->type == BlockType::Air) return true;
	Block* west = getBlock(block.x - 1, block.y, block.z);
	if (west == nullptr || west->type == BlockType::Air) return true;
	Block* east = getBlock(block.x + 1, block.y, block.z);
	if (east == nullptr || east->type == BlockType::Air) return true;
	Block* front = getBlock(block.x, block.y + 1, block.z - 1);
	if (front == nullptr || front->type == BlockType::Air) return true;
	Block* back = getBlock(block.x, block.y, block.z + 1);
	if (back == nullptr || back->type == BlockType::Air) return true;
	return false;
}
