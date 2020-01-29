#pragma once
#include "Chunk.h"
#include <map>
#include "Position.h"
#include "MathFunctions.h"
#include <mutex>
#include <random>
#include <ctime>

namespace BiomeMgr {
	class BiomeManager
	{
	public:
		BiomeManager() { gen.seed(time(0)); };
		~BiomeManager() = default;
		Chunk::Biome operator[](Position2 pos);
	private:
		Chunk::Biome DetermineBiome(const Position2& pos) noexcept;
		inline Position2 Clip(const Position2& pos) const noexcept;
		std::map<Position2, Chunk::Biome> biomeMap;
		std::mutex accessMutex;

		static constexpr int AreaSize = 20 * Chunk::ChunkSize;
		static constexpr int biomeCount = static_cast<int>(Chunk::Biome::Biome_count) - 1;

		std::random_device rd;
		std::mt19937 gen{rd()};
		std::uniform_int_distribution<> dis{0, biomeCount};
	};

	inline BiomeManager biomeManager;
}

