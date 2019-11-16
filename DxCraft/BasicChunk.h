#pragma once
#include "Block.h"
#include <vector>
#include <unordered_map>

struct Position {
	Position(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
	Position() = delete;
	bool operator==(const Position& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
	int x, y, z;
};

struct PositionHash {
	std::size_t operator()(const Position& position) const {
		std::hash<int> hash;
		return ((hash(position.x) ^ hash(position.y)) << 1) ^ (hash(position.z) << 1);
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
	static constexpr int chunkSize = 16;
private:
	Position Normalize(int x, int y, int z) const noexcept;
	inline int FlatIndex(int x, int y, int z) const noexcept;
	std::vector<Block> blocks;
	int x, y, z;
};

