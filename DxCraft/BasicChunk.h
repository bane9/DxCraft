#pragma once
#include "Block.h"
#include <vector>
#include <unordered_map>


struct Position {
	Position(int x, int y, int z) : x(x), y(y), z(z) {}
	Position() = default;
	int x, y, z;
};

struct PositionHash {
	std::size_t operator()(const Position& position) const {
		std::hash<int> hash;
		return ((hash(position.x) ^ hash(position.y)) << 1) ^ (hash(position.z) << 1);
	}
};

struct PositionComparator {
	bool operator()(const Position& first, const Position& second) const {
		return first.x == second.x && first.y == second.y && first.z == second.z;
	}
};

class BasicChunk
{
	friend class WorldManager;
public:
	BasicChunk(int x, int y, int z);
	void AddBlock(int x, int y, int z, BlockType type) noexcept;
	void RemoveBlock(int x, int y, int z) noexcept;
	Block& GetBlock(int x, int y, int z) noexcept;
	Position GetPosition() const noexcept;

private:
	Position Normalize(int x, int y, int z) const noexcept;
	inline int FlatIndexPure(int x, int y, int z) noexcept;
	inline int FlatIndex(int x, int y, int z) const noexcept;
	static constexpr int chunkSize = 16;
	std::vector<Block> blocks;
	int x, y, z;
};

