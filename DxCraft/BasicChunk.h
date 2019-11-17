#pragma once
#include "Block.h"
#include <vector>
#include <unordered_map>
#include "XM_Structs.h"

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
		return hash(position.x) ^ hash(position.y) ^ hash(position.z);
	}
};

class BasicChunk
{
	friend class WorldManager;
	friend class MeshRenderer;
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
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	int x, y, z;
};

