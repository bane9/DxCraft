#pragma once
#include <array>
#include "Position.h"
#include "AABB.h"

class Block {
	friend class BasicChunk;
public:
	enum class BlockType {
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

		block_count,
		None
	};

	enum class MeshType {
		INDEPENDANT_CUBE,
		FULL_MESH_L,
		FULL_MESH_M,
		FULL_MESH_S,
		SAPLING
	};

	enum class SelectorType {
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

	Position GetPosition() const noexcept;
	BlockType GetBlockType() const noexcept;
	bool IsTransparent() const noexcept;
	bool IsCollideable() const noexcept;
	bool IsFullMesh() const noexcept;
	MeshType GetMeshType() const noexcept;
	SelectorType GetSelectorType() const noexcept;
	void SetBlockType(BlockType type) noexcept;
	const std::array<std::array<float, 2>, 6>& GetTexCoords() const noexcept;
	AABB GetAABB();

private:
	Position pos;
	BlockType blockType;
	MeshType meshType;
	SelectorType selectorType;
};
