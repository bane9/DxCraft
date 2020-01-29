#include "Chunk.h"
#include "MathFunctions.h"

Chunk::Chunk(int x, int y, int z)
	: x(x), y(y), z(z), aabb({ 16.0f, 16.0f, 16.0f })
{
	aabb.SetPosition({ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) });
}

Position Chunk::GetPosition() const noexcept
{
	return { x, y, z };
}

Block& Chunk::operator()(int x, int y, int z)
{
	return blocks[FlatIndex(x, y, z)];
}

Chunk::Biome Chunk::GetBiome()
{
	return biome;
}

inline Position Chunk::Normalize(int x, int y, int z) const noexcept
{
	return { FixedMod(x - this->x, ChunkSize), FixedMod(y - this->y, ChunkSize), FixedMod(z - this->z, ChunkSize) };
}

inline int Chunk::FlatIndex(int x, int y, int z) const noexcept
{
	Position p = Normalize(x, y, z);
	return p.x + ChunkSize * (p.y + ChunkSize * p.z);
}
