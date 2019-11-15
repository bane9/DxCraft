#pragma once
#include "Block.h"
#include <vector>

struct Position {
	int x, y, z;
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
	inline int FlatIndex(int x, int y, int z) const noexcept;
	std::vector<Block> blocks;
	int x, y, z;
};

