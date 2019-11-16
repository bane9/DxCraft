#include "BasicChunk.h"
#include <math.h>

BasicChunk::BasicChunk(int x, int y, int z)
	: x(x), y(y), z(z)
{
	blocks.resize(chunkSize * chunkSize * chunkSize);
	for (int ix = 0; ix < chunkSize; ix++) {
		for (int iy = 0; iy < chunkSize; iy++) {
			for (int iz = 0; iz < chunkSize; iz++) {
				//if (iy % 5 == 0) continue;
				Block& block = blocks[FlatIndex(ix, iy, iz)];
				block.x = x + ix;
				block.y = y + iy;
				block.z = z + iz;
				block.type = BlockType::Grass;
			}
		}
	}
}

void BasicChunk::AddBlock(int x, int y, int z, BlockType type) noexcept
{
	blocks[FlatIndex(x, y, z)].type = type;
}

void BasicChunk::RemoveBlock(int x, int y, int z) noexcept
{
	blocks[FlatIndex(x, y, z)].type = BlockType::Air;
}

Block& BasicChunk::GetBlock(int x, int y, int z) noexcept
{
	return blocks[FlatIndex(x, y, z)];
}

Position BasicChunk::GetPosition() const noexcept
{
	return { x, y, z };
}

inline Position BasicChunk::Normalize(int x, int y, int z) const noexcept
{
	return { std::abs(x - this->x) % chunkSize, std::abs(y - this->y) % chunkSize, std::abs(z - this->z) % chunkSize };
}

inline int BasicChunk::FlatIndex(int x, int y, int z) const noexcept
{
	Position p = Normalize(x, y, z);
	return p.x + chunkSize * (p.y + chunkSize * p.z);
}
