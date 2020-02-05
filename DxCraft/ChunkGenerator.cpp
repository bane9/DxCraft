#include "ChunkGenerator.h"
#include <algorithm>

using bInfo = ChunkGenerator::BiomeInfo;
static constexpr std::array<bInfo, static_cast<int>(Chunk::Biome::Biome_count)> BiomeValues = {
	bInfo{2.0f, 25.0f}, //Dirt
	bInfo{3.0f, 30.0f}, //Grass
	bInfo{2.0f, 27.0f}, //Dirt
	bInfo{2.0f, 20.0f}, //Stone
};

static auto hMapIndex = [](int x, int y) {
	return y * Chunk::ChunkSize + x;
};

void ChunkGenerator::ProccessChunk(chunkArray& chunkArea)
{
	std::shared_ptr<Chunk> inheritsFrom = nullptr;
	auto& origin = *chunkArea[ChunkPosition::Origin];
	origin.biome = ChunkGenerator::GetBiome(origin.x, origin.z);
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
	//default:
	case Chunk::Biome::Stone:
		GenerateStoneChunk(chunkArea);
		break;
	}
}

Chunk::Biome ChunkGenerator::GetBiome(int x, int z)
{
	static FastNoise noise;
	static bool doOnce = false;
	if (!doOnce) {
		noise.SetNoiseType(FastNoise::Simplex);
		noise.SetFrequency(0.0001f);
		doOnce = true;
	}
	return static_cast<Chunk::Biome>(ValueMap(noise.GetNoise(x, z), -1.0f, 1.0f, 0, static_cast<int>(Chunk::Biome::Biome_count)));
}

void ChunkGenerator::GenerateGrassChunk(const chunkArray& chunkArea)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
				//else if (chunk.heightMap[hMapIndex(x, z)] - chunk.y + y < Chunk::ChunkSize + 2 && chunk.y + y < (worldScale * Chunk::ChunkSize) * 0.85f) chunk(x, y, z).SetBlockType(Block::BlockType::Sand);
				else chunk(x, y, z).SetBlockType(Block::BlockType::Grass);
			}
		}
	}
}

void ChunkGenerator::GenerateSandChunk(const chunkArray& chunkArea)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Sand);
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
			}
		}
	}
}

void ChunkGenerator::GenerateDirtChunk(const chunkArray& chunkArea)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea);
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
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();
	GenerateHeightMap(chunkArea);
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			for (int y = 0; y < std::clamp((int)chunk.heightMap[hMapIndex(x, z)] - chunk.y, 0, Chunk::ChunkSize); y++) {
				chunk(x, y, z).SetBlockType(Block::BlockType::Oak_Wood);
				if (chunk.y + y == 0) chunk(x, y, z).SetBlockType(Block::BlockType::Bedrock);
			}
		}
	}
}

void ChunkGenerator::GenerateHeightMap(const chunkArray& chunkArea)
{
	auto& chunk = *chunkArea[ChunkPosition::Origin];
	Position pos = chunk.GetPosition();

	chunk.biome = ChunkGenerator::GetBiome(chunk.x, chunk.z);
	bInfo biomeInfo = BiomeValues[static_cast<int>(chunk.biome)];

	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			chunk.heightMap[hMapIndex(x, z)] = biomeInfo.prescale +
				std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * worldScaler * biomeInfo.worldScale - 1,
					0.0f,
					255.0f - biomeInfo.prescale);
		}
	}

	if (chunkArea[ChunkPosition::Bottom] != nullptr && chunkArea[ChunkPosition::Bottom]->HasGenerated) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = Lerp(
					chunkArea[ChunkPosition::Bottom]->heightMap[hMapIndex(x, Chunk::ChunkSize - 1)],
					chunk.heightMap[hMapIndex(x, Chunk::ChunkSize - 1)],
					(float)z / Chunk::ChunkSize
				);
			}
		}
	}
	else if (chunkArea[ChunkPosition::Top] != nullptr && chunkArea[ChunkPosition::Top]->HasGenerated) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = Lerp(
					chunk.heightMap[hMapIndex(x, 0)],
					chunkArea[ChunkPosition::Top]->heightMap[hMapIndex(x, 0)],
					(float)z/ Chunk::ChunkSize
				);
			}
		}
	}
	

	if (chunkArea[ChunkPosition::Left] != nullptr && chunkArea[ChunkPosition::Left]->HasGenerated) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = Lerp(
					chunkArea[ChunkPosition::Left]->heightMap[hMapIndex(Chunk::ChunkSize - 1, z)],
					chunk.heightMap[hMapIndex(Chunk::ChunkSize - 1, z)],
					(float)x / Chunk::ChunkSize
				);
			}
		}
	}
	else if (chunkArea[ChunkPosition::Right] != nullptr && chunkArea[ChunkPosition::Right]->HasGenerated) {
		for (int x = 0; x < Chunk::ChunkSize; x++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				chunk.heightMap[hMapIndex(x, z)] = Lerp(
					chunk.heightMap[hMapIndex(0, z)],
					chunkArea[ChunkPosition::Right]->heightMap[hMapIndex(0, z)],
					(float)x / Chunk::ChunkSize
				);
			}
		}
	}
}
