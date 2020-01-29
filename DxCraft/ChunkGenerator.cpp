#include "ChunkGenerator.h"
#include <algorithm>
#include "BiomeManager.h"

void ChunkGenerator::ProccessChunk(chunkArray& chunkArea)
{
	std::shared_ptr<Chunk> inheritsFrom = nullptr;
	auto& origin = *chunkArea[ChunkPosition::Origin];
	origin.biome = BiomeMgr::biomeManager[{origin.x, origin.z}];

	switch (origin.biome)
	{
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
	constexpr float worldScale = 5;
	constexpr float prescale = 35;
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			float height = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * Chunk::ChunkSize * worldScale - 1,
				0.0f,
				255.0f - prescale);
			for (int y = 0; y < std::clamp((int)height - chunk.y, 0, Chunk::ChunkSize); y++) {
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
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			float height = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * Chunk::ChunkSize * worldScale - 1,
				0.0f,
				255.0f - prescale);
			for (int y = 0; y < std::clamp((int)height - chunk.y, 0, Chunk::ChunkSize); y++) {
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
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			float height = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * Chunk::ChunkSize * worldScale - 1,
				0.0f,
				255.0f - prescale);
			for (int y = 0; y < std::clamp((int)height - chunk.y, 0, Chunk::ChunkSize); y++) {
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
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			float height = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * Chunk::ChunkSize * worldScale - 1,
				0.0f,
				255.0f - prescale);
			for (int y = 0; y < std::clamp((int)height - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Stone);
			}
		}
	}
}
