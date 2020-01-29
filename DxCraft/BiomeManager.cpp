#include "BiomeManager.h"

Chunk::Biome BiomeMgr::BiomeManager::operator[](Position2 pos)
{
	std::unique_lock<std::mutex> lock(accessMutex);
	Position2 clipped = Clip(pos);
	auto biome = biomeMap.find(clipped);
	if (biome != biomeMap.end()) return biome->second;
	else return biomeMap[clipped] = static_cast<Chunk::Biome>(dis(gen));
}

Chunk::Biome BiomeMgr::BiomeManager::DetermineBiome(const Position2& pos) noexcept
{
	return {}; //For future placeholder
}

inline Position2 BiomeMgr::BiomeManager::Clip(const Position2& pos) const noexcept
{
	return Position2(pos.x / AreaSize, pos.z / AreaSize);
}
