#include "BasicChunk.h"
#include "MathFunctions.h"

BasicChunk::BasicChunk(int x, int y, int z, bool empty)
	: x(x), y(y), z(z), aabb({16.0f, 16.0f, 16.0f})
{
	blocks.resize(chunkSize * chunkSize * chunkSize);
	for (int ix = 0; ix < chunkSize; ix++) {
		for (int iy = 0; iy < chunkSize; iy++) {
			for (int iz = 0; iz < chunkSize; iz++) {
				auto& block = blocks[FlatIndex(ix, iy, iz)];
				block.pos.x = ix + x;
				block.pos.y = iy + y;
				block.pos.z = iz + z;
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
	return { FixedMod(x - this->x, chunkSize), FixedMod(y - this->y, chunkSize), FixedMod(z - this->z, chunkSize) };
}

inline int BasicChunk::FlatIndex(int x, int y, int z) const noexcept
{
	Position p = Normalize(x, y, z);
	return p.x + chunkSize * (p.y + chunkSize * p.z);
}
