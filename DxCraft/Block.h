#pragma once
#include <array>
#include "Position.h"
#include "AABB.h"

class Block {
	friend class Chunk;
	friend class WorldGenerator;
public:
	enum class BlockType : uint8_t {
		Air,
		Stone,
		Grass,
		Dirt,
		Wooden_Plank,
		Bedrock,
		Glass,
		Sugar_Cane,
		Poppy,
		Dandelion,
		Red_Mushroom,
		Brown_Mushroom,
		Oak_Sapling,
		Birch_Sapling,
		Dark_Oak_Sapling,
		Oak_Wood,
		Leaves,
		Water,
		Lava,
		Sand,

		block_count,
		None
	};

	enum class MeshType : uint8_t {
		INDEPENDANT_CUBE,
		FULL_MESH_L,
		FULL_MESH_M,
		FULL_MESH_S,
		SAPLING
	};

	enum class SelectorType : uint8_t {
		BLOCK,
		BILBOARD_FULL_L,
		MUSHROOM,
		FLOWER,
		SAPLING
	};

public:
	Block();
	Block(int x, int y, int z, BlockType blockType = BlockType::Air);
	Block(Position pos, BlockType blockType = BlockType::Air);

	~Block() = default;

	BlockType GetBlockType() const noexcept;
	bool IsTransparent() const noexcept;
	bool IsCollideable() const noexcept;
	bool IsFullMesh() const noexcept;
	MeshType GetMeshType() const noexcept;
	SelectorType GetSelectorType() const noexcept;
	void SetBlockType(Block::BlockType type) noexcept;
	const std::array<std::array<float, 2>, 6>& GetTexCoords() const noexcept;
	AABB GetAABB() const noexcept;
	bool NeedsSeperateDrawCall() const noexcept;
	bool IsLiquid() const noexcept;
private:
	BlockType blockType = Block::BlockType::Air;
};
