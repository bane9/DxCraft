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
	noise.SetFrequency(0.001f);
	origin.biome = static_cast<Chunk::Biome>(
		ValueMap(noise.GetNoise(origin.x, origin.z), -1.0f, 1.0f, 0, static_cast<int>(Chunk::Biome::Biome_count))
		);

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
	noise.SetNoiseType(FastNoise::NoiseType::SimplexFractal);
	noise.SetFrequency(0.01f);
	noise.SetFractalOctaves(2);
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	constexpr float worldScale = 6;
	constexpr float prescale = 20;
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Grass);
			}
		}
	}
}

void ChunkGenerator::GenerateSandChunk(const chunkArray& chunkArea)
{
	noise.SetNoiseType(FastNoise::NoiseType::Value);
	noise.SetFrequency(0.01f);
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	constexpr float worldScale = 3;
	constexpr float prescale = 30;
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Sand);
			}
		}
	}
}

void ChunkGenerator::GenerateDirtChunk(const chunkArray& chunkArea)
{
	noise.SetNoiseType(FastNoise::NoiseType::PerlinFractal);
	noise.SetFrequency(0.01f);
	noise.SetFractalOctaves(3);
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	constexpr float worldScale = 2;
	constexpr float prescale = 27;
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Dirt);
			}
		}
	}
}

void ChunkGenerator::GenerateStoneChunk(const chunkArray& chunkArea)
{
	noise.SetNoiseType(FastNoise::NoiseType::Perlin);
	noise.SetFrequency(0.025f);
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	constexpr float worldScale = 2;
	constexpr float prescale = 20;
	GenerateHeightMap(chunkArea, worldScale, prescale);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Oak_Wood);
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
			chunk.heightMap[hMapIndex(x, z)] = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * Chunk::ChunkSize * worldScale - 1,
				0.0f,
				255.0f - prescale);
		}
	}


	if (chunkArea[ChunkPosition::Bottom] != nullptr && chunkArea[ChunkPosition::Bottom]->HasGenerated 
		&& chunk.biome != chunkArea[ChunkPosition::Bottom]->biome) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				auto& hOrigin = chunk.heightMap[hMapIndex(x, Chunk::ChunkSize - 1 - z)];
				auto hTarget = chunkArea[ChunkPosition::Bottom]->heightMap[hMapIndex(x, z)];
				hOrigin = Lerp(hOrigin, hTarget, (float)z / (float)Chunk::ChunkSize);
			}
		}
	}

	if (chunkArea[ChunkPosition::Top] != nullptr && chunkArea[ChunkPosition::Top]->HasGenerated 
		&& chunk.biome != chunkArea[ChunkPosition::Top]->biome) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				auto& hOrigin = chunk.heightMap[hMapIndex(x, z)];
				auto hTarget = chunkArea[ChunkPosition::Top]->heightMap[hMapIndex(x, Chunk::ChunkSize - 1 - z)];
				hOrigin = Lerp(hOrigin, hTarget, (float)z / (float)Chunk::ChunkSize);
			}
		}
	}

	if (chunkArea[ChunkPosition::Left] != nullptr && chunkArea[ChunkPosition::Left]->HasGenerated
		&& chunk.biome != chunkArea[ChunkPosition::Left]->biome) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				auto& hOrigin = chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1 - x, z)];
				auto hTarget = chunkArea[ChunkPosition::Left]->heightMap[hMapIndex(x, z)];
				hOrigin = Lerp(hOrigin, hTarget, (float)x / (float)Chunk::ChunkSize);
			}
		}
	}

	if (chunkArea[ChunkPosition::Right] != nullptr && chunkArea[ChunkPosition::Right]->HasGenerated 
		&& chunk.biome != chunkArea[ChunkPosition::Right]->biome) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				auto& hOrigin = chunk.heightMap[hMapIndex(x, z)];
				auto hTarget = chunkArea[ChunkPosition::Right]->heightMap[hMapIndex(Chunk::ChunkSize - 1 - x, z)];
				hOrigin = Lerp(hOrigin, hTarget, (float)x / (float)Chunk::ChunkSize);
			}
		}
	}
	
}
