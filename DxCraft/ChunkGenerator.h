#pragma once
#include "Chunk.h"
#include "FastNoise.h"
#include "MathFunctions.h"
#include <array>
#include <memory>
#include <random>
#include <ctime>

class ChunkGenerator
{
public:
	enum ChunkPosition {
		Top,
		Bottom,
		Origin,
		Left,
		Right
	};
	using chunkArray = std::array<std::shared_ptr<Chunk>, ChunkPosition::Right + 1>;
	ChunkGenerator() { gen.seed(time(0)); noise.SetSeed(time(0)); };
	~ChunkGenerator() = default;
	void ProccessChunk(chunkArray& chunkArea);
	static Chunk::Biome GetBiome(int x, int z);
	struct BiomeInfo {
		float worldScale;
		float prescale;
	};
private:
	void GenerateGrassChunk(const chunkArray& chunkArea);
	void GenerateSandChunk(const chunkArray& chunkArea);
	void GenerateDirtChunk(const chunkArray& chunkArea);
	void GenerateStoneChunk(const chunkArray& chunkArea);
	void GenerateHeightMap(const chunkArray& chunkArea);

	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> dis{ 0, static_cast<int>(Chunk::Biome::Biome_count) };

	FastNoise noise;

	static constexpr int worldScaler = 16;
};
