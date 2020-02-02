#include "ChunkGenerator.h"
#include <algorithm>

static auto hMapIndex = [](int x, int y) {
	return y * Chunk::ChunkSize + x;
};

void ChunkGenerator::ProccessChunk(chunkArray& chunkArea)
{
	std::shared_ptr<Chunk> inheritsFrom = nullptr;
	auto& origin = *chunkArea[ChunkPosition::Origin];
	noise.SetNoiseType(FastNoise::NoiseType::Simplex);
	
	noise.SetFrequency(0.0001f);
	origin.biome = static_cast<Chunk::Biome>(
		ValueMap(noise.GetNoise(origin.x, origin.z), -1.0f, 1.0f, 0, static_cast<int>(Chunk::Biome::Biome_count))
		);
	noise.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
	noise.SetFrequency(0.005f);
	noise.SetFractalOctaves(2);
	switch (origin.biome)
	{
	default:
	case Chunk::Biome::Grass:
		GenerateGrassChunk(chunkArea);
		break;
	case Chunk::Biome::Sand:
		GenerateSandChunk(chunkArea);
		break;
	case Chunk::Biome::Dirt:
		GenerateDirtChunk(chunkArea);
		break;
	case Chunk::Biome::Stone:
		GenerateStoneChunk(chunkArea);
		break;
	}
}

void ChunkGenerator::GenerateGrassChunk(const chunkArray& chunkArea)
{
	/*noise.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
	noise.SetFrequency(0.01f);
	noise.SetFractalOctaves(2);
	constexpr float worldScale = 3;
	constexpr float prescale = 20;*/
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
				else if (chunk.heightMap[hMapIndex(x, z)] - chunk.y + y < Chunk::ChunkSize && chunk.y + y < (worldScale * Chunk::ChunkSize) * 0.85f) chunk(x, y, z).SetBlockType(Block::BlockType::Sand);
				else chunk(x, y, z).SetBlockType(Block::BlockType::Grass);
			}
			for (int y = 0; y < Chunk::ChunkSize; y++) {
				if (chunk.y + y < (worldScale * Chunk::ChunkSize) * 0.8f && chunk(x, y, z).GetBlockType() == Block::BlockType::Air) {
					chunk(x, y, z).SetBlockType(Block::BlockType::Water);
				}
			}
		}
	}
}

void ChunkGenerator::GenerateSandChunk(const chunkArray& chunkArea)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Sand);
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
			}
			for (int y = 0; y < Chunk::ChunkSize; y++) {
				if (chunk.y + y < (worldScale * Chunk::ChunkSize) * 0.75f && chunk(x, y, z).GetBlockType() == Block::BlockType::Air) chunk(x, y, z).SetBlockType(Block::BlockType::Water);
			}
		}
	}
}

void ChunkGenerator::GenerateDirtChunk(const chunkArray& chunkArea)
{
	/*noise.SetNoiseType(FastNoise::NoiseType::PerlinFractal);
	noise.SetFrequency(0.01f);
	noise.SetFractalOctaves(3);
	constexpr float worldScale = 2;
	constexpr float prescale = 27;*/
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Dirt);
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
			}
		}
	}
}

void ChunkGenerator::GenerateStoneChunk(const chunkArray& chunkArea)
{
	/*noise.SetNoiseType(FastNoise::NoiseType::Perlin);
	noise.SetFrequency(0.025f);
	constexpr float worldScale = 2;
	constexpr float prescale = 20;*/
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Oak_Wood);
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
			}
		}
	}
}

void ChunkGenerator::GenerateHeightMap(const chunkArray& chunkArea, int worldScale, int prescale)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();

	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			chunk.heightMap[hMapIndex(x, z)] = prescale + 
				std::clamp(
					(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * worldScaler * worldScale - 1,
					0.0f,
					255.0f - prescale);
		}
	}
	return;

	if (chunkArea[ChunkPosition::Top] != nullptr && chunkArea[ChunkPosition::Top]->HasGenerated) {
		float bottomLeft = chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1, 0)];
		float topLeft = chunkArea[ChunkPosition::Top]->heightMap[hMapIndex(0, 0)];
		float bottomRight = chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1, Chunk::ChunkSize - 1)];
		float topRight = chunkArea[ChunkPosition::Top]->heightMap[hMapIndex(0, Chunk::ChunkSize - 1)];

		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = BilinearInterpolation(
					bottomLeft, topLeft, bottomRight, topRight,
					0, Chunk::ChunkSize - 1,
					0, Chunk::ChunkSize - 1,
					x, z
				);
			}
		}
	}

	if (chunkArea[ChunkPosition::Right] != nullptr && chunkArea[ChunkPosition::Right]->HasGenerated) {
		float bottomLeft = chunk.heightMap[hMapIndex(0, 0)];
		float topLeft = chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1, 0)];
		float bottomRight = chunkArea[ChunkPosition::Right]->heightMap[hMapIndex(0, 0)];
		float topRight = chunkArea[ChunkPosition::Right]->heightMap[hMapIndex(Chunk::ChunkSize - 1, 0)];

		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = BilinearInterpolation(
					bottomLeft, topLeft, bottomRight, topRight,
					0, Chunk::ChunkSize - 1,
					0, Chunk::ChunkSize - 1,
					x, z
				);
			}
		}
	}

	if (chunkArea[ChunkPosition::Left] != nullptr && chunkArea[ChunkPosition::Left]->HasGenerated) {
		float bottomLeft = chunkArea[ChunkPosition::Left]->heightMap[hMapIndex(Chunk::ChunkSize - 1, 0)];
		float topLeft = chunkArea[ChunkPosition::Left]->heightMap[hMapIndex(Chunk::ChunkSize - 1, Chunk::ChunkSize - 1)];
		float bottomRight = chunk.heightMap[hMapIndex(0, 0)];
		float topRight = chunk.heightMap[hMapIndex(0, Chunk::ChunkSize - 1)];

		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = BilinearInterpolation(
					bottomLeft, topLeft, bottomRight, topRight,
					0, Chunk::ChunkSize - 1,
					0, Chunk::ChunkSize - 1,
					x, z
				);
			}
		}
	}


	if (false && chunkArea[ChunkPosition::Bottom] != nullptr && chunkArea[ChunkPosition::Bottom]->HasGenerated) {
		float bottomLeft = chunk.heightMap[hMapIndex(0, 0)];
		float topLeft = chunkArea[ChunkPosition::Bottom]->heightMap[hMapIndex(0, Chunk::ChunkSize - 1)];
		float bottomRight = chunkArea[ChunkPosition::Bottom]->heightMap[hMapIndex(Chunk::ChunkSize - 1, Chunk::ChunkSize - 1)];
		float topRight = chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1, 0)];

		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = BilinearInterpolation(
					bottomLeft, topLeft, bottomRight, topRight,
					0, Chunk::ChunkSize - 1,
					0, Chunk::ChunkSize - 1,
					x, z
				);
			}
		}
	}
}
