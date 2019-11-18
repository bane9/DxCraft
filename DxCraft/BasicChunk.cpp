#include "BasicChunk.h"
#include <math.h>

BasicChunk::BasicChunk(int x, int y, int z)
	: x(x), y(y), z(z)
{
	blocks.resize(chunkSize * chunkSize * chunkSize);
	for (int ix = 0; ix < chunkSize; ix++) {
		for (int iy = 0; iy < chunkSize; iy++) {
			for (int iz = 0; iz < chunkSize; iz++) {
				Block& block = blocks[FlatIndex(ix, iy, iz)];
				block.x = x + ix;
				block.y = y + iy;
				block.z = z + iz;
				block.type = BlockType::Grass;
				//if((iy + ix + iz) % 2 == 0) block.type = BlockType::Air;
			}
		}
	}
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
