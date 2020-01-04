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
		}},
		{{ //Poppy
			{12, 0},
			{12, 0},
			{12, 0},
			{12, 0},
			{12, 0},
			{12, 0},
		}},
		{{ //Dandelion
			{13, 0},
			{13, 0},
			{13, 0},
			{13, 0},
			{13, 0},
			{13, 0},
		}},
		{{ //Red mushroom
			{12, 1},
			{12, 1},
			{12, 1},
			{12, 1},
			{12, 1},
			{12, 1},
		}},
		{{ //Brown mushroom
			{13, 1},
			{13, 1},
			{13, 1},
			{13, 1},
			{13, 1},
			{13, 1},
		}},
		{{ //Oak sapling
			{15, 0},
			{15, 0},
			{15, 0},
			{15, 0},
			{15, 0},
			{15, 0},
		}},
		{{ //Brown mushroom
			{15, 3},
			{15, 3},
			{15, 3},
			{15, 3},
			{15, 3},
			{15, 3},
		}},
		{{ //Brown mushroom
			{15, 4},
			{15, 4},
			{15, 4},
			{15, 4},
			{15, 4},
			{15, 4},
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
	switch (blockType)
	{
	case Block::BlockType::Air:
	case Block::BlockType::Glass:
	case Block::BlockType::Sugar_Cane:
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return true;
	default:
		return false;
	}
}


bool Block::IsCollideable() const noexcept
{
	switch (blockType)
	{
	case Block::BlockType::Air:
	case Block::BlockType::Sugar_Cane:
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return false;
	default:
		return true;
	}
}

bool Block::IsFullMesh() const noexcept
{
	return meshType != Block::MeshType::INDEPENDANT_CUBE;
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

	switch (type) {
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		meshType = Block::MeshType::SAPLING;
		selectorType = Block::SelectorType::SAPLING;
		break;
	case Block::BlockType::Sugar_Cane:
		meshType = Block::MeshType::FULL_MESH_L;
		selectorType = Block::SelectorType::BILBOARD_FULL_L;
		break;
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
		meshType = Block::MeshType::FULL_MESH_S;
		selectorType = Block::SelectorType::FLOWER;
		break;
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		meshType = Block::MeshType::FULL_MESH_S;
		selectorType = Block::SelectorType::MUSHROOM;
		break;
	default:
		meshType = Block::MeshType::INDEPENDANT_CUBE;
		selectorType = Block::SelectorType::BLOCK;
		break;
	}

}

const std::array<std::array<float, 2>, 6>& Block::GetTexCoords() const noexcept
{
	return BlockTextures[static_cast<int>(blockType)];
}

AABB Block::GetAABB()
{
	switch (selectorType) {
	case Block::SelectorType::BILBOARD_FULL_L:
		return AABB({ 0.4f, 0.5f, 0.4f });
	case Block::SelectorType::FLOWER:
		return AABB({ 0.25f, 0.35f, 0.25f });
	case Block::SelectorType::MUSHROOM:
		return AABB({ 0.25f, 0.25f, 0.25f });
	default:
		return AABB({ 0.5f, 0.5f, 0.5f });
	}
}
