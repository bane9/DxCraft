#include "BasicChunk.h"
#include "MathFunctions.h"
#include "RenderDataFactory.h"

BasicChunk::BasicChunk(int x, int y, int z, bool empty)
	: x(x), y(y), z(z), aabb({16.0f, 16.0f, 16.0f})
{
	aabb.SetPosition({static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)});
	blocks.resize(chunkSize * chunkSize * chunkSize);
	for (int ix = 0; ix < chunkSize; ix++) {
		for (int iy = 0; iy < chunkSize; iy++) {
			for (int iz = 0; iz < chunkSize; iz++) {
				Block& block = blocks[FlatIndex(ix, iy, iz)];
				block.x = x + ix;
				block.y = y + iy;
				block.z = z + iz;
				if (iy == 15)
					block.type = BlockType::Grass;
				else if (iy >= 12 && iy < 15)
					block.type = BlockType::Dirt;
				else if (iy > 0 && iy < 12)
					block.type = BlockType::Stone;
				else
					block.type = BlockType::Bedrock;
				if (empty) block.type = BlockType::Air;

				//block.type = static_cast<BlockType>(rand() % static_cast<int>(BlockType::block_count));

				/*if ((ix + iy + iz) % 2 == 0) block.type = BlockType::Air;
				else block.type = BlockType::Grass;*/
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
