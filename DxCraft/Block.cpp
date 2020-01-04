#include "Block.h"

static constexpr std::array<std::array<std::array<float, 2>, 6>, static_cast<int>(Block::BlockType::block_count)> BlockTextures
{ {
		/*
		Far
		Near
		Left
		Right
		Bottom
		Top
		*/
		{}, //Air
		{{ //Stone
			{1, 0},
			{1, 0},
			{1, 0},
			{1, 0},
			{1, 0},
			{1, 0},
		}},
		{{ //Grass
			{3, 0},
			{3, 0},
			{3, 0},
			{3, 0},
			{2, 0},
			{0, 0},
		}},
		{{ //Dirt
			{2, 0},
			{2, 0},
			{2, 0},
			{2, 0},
			{2, 0},
			{2, 0},
		}},
		{{ //Wodden Plank
			{4, 0},
			{4, 0},
			{4, 0},
			{4, 0},
			{4, 0},
			{4, 0},
		}},
		{{ //Bedrock
			{1, 1},
			{1, 1},
			{1, 1},
			{1, 1},
			{1, 1},
			{1, 1},
		}},
		{{ //Glass
			{1, 3},
			{1, 3},
			{1, 3},
			{1, 3},
			{1, 3},
			{1, 3},
		}},
		{{ //Sugar cane
			{9, 4},
			{9, 4},
			{9, 4},
			{9, 4},
			{9, 4},
			{9, 4},
		}}

}};


Block::Block()
	: pos(0, 0, 0)
{
}

Block::Block(int x, int y, int z, BlockType blockType)
	: pos(x, y, z)
{
	SetBlockType(blockType);
}

Block::Block(Position pos, BlockType blockType)
	: pos(pos)
{
	SetBlockType(blockType);
}

Position Block::GetPosition() const noexcept
{
	return pos;
}

Block::BlockType Block::GetBlockType() const noexcept
{
	return blockType;
}

bool Block::IsTransparent() const noexcept
{
	return (blockType == Block::BlockType::Air ||
			blockType == Block::BlockType::Glass || 
		blockType == Block::BlockType::Sugar_Cane);
}

bool Block::IsCollideable() const noexcept
{
	return !(blockType == Block::BlockType::Air || blockType == Block::BlockType::Sugar_Cane);
}

bool Block::IsFullMesh() const noexcept
{
	return meshType == Block::MeshType::FULL_MESH;
}

Block::MeshType Block::GetMeshType() const noexcept
{
	return meshType;
}

Block::SelectorType Block::GetSelectorType() const noexcept
{
	return selectorType;
}

void Block::SetBlockType(BlockType type) noexcept
{
	blockType = type;

	if (type == Block::BlockType::Sugar_Cane)
		meshType = Block::MeshType::FULL_MESH;
	else
		meshType = Block::MeshType::INDEPENDANT_CUBE;

	if (type == Block::BlockType::Sugar_Cane)
		selectorType = Block::SelectorType::BILBOARD_FULL;
	else
		selectorType = Block::SelectorType::BLOCK;

}

const std::array<std::array<float, 2>, 6>& Block::GetTexCoords() const noexcept
{
	return BlockTextures[static_cast<int>(blockType)];
}

AABB Block::GetAABB()
{
	switch (selectorType) {
	case Block::SelectorType::BILBOARD_FULL:
		return AABB({ 0.8f, 0.0f, 0.8f });
	default:
		return AABB({ 1.0f, 1.0f, 1.0f });
	}
}
