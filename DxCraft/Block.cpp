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
		}},
		{{ //Oak wood
			{4, 1},
			{4, 1},
			{4, 1},
			{4, 1},
			{5, 1},
			{5, 1},
		}},
		{{ //Leaves
			{4, 3},
			{4, 3},
			{4, 3},
			{4, 3},
			{4, 3},
			{4, 3},
		}},
		{{ //Water
			{14, 13},
			{14, 13},
			{14, 13},
			{14, 13},
			{14, 13},
			{14, 13},
		}},
		{{ //Lava
			{14, 15},
			{14, 15},
			{14, 15},
			{14, 15},
			{14, 15},
			{14, 15},
		}}

}};


Block::Block()
{
}

Block::Block(int x, int y, int z, BlockType blockType)
	: blockType(blockType)
{
}

Block::Block(Position pos, BlockType blockType)
	: blockType(blockType)
{
}

Block::BlockType Block::GetBlockType() const noexcept
{
	return blockType;
}


bool Block::IsTransparent() const noexcept
{
	if (this == nullptr) return false; //TODO: Fix later lmao
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
	case Block::BlockType::Leaves:
	case Block::BlockType::Water:
	case Block::BlockType::Lava:
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
	case Block::BlockType::Water:
	case Block::BlockType::Lava:
		return false;
	default:
		return true;
	}
}

bool Block::IsFullMesh() const noexcept
{
	return GetMeshType() != Block::MeshType::INDEPENDANT_CUBE;
}

Block::MeshType Block::GetMeshType() const noexcept
{
	switch (blockType) {
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return Block::MeshType::SAPLING;
		break;
	case Block::BlockType::Sugar_Cane:
		return Block::MeshType::FULL_MESH_L;
		break;
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
		return Block::MeshType::FULL_MESH_S;
		break;
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		return Block::MeshType::FULL_MESH_S;
		break;
	default:
		return Block::MeshType::INDEPENDANT_CUBE;
		break;
	}
}

Block::SelectorType Block::GetSelectorType() const noexcept
{
	switch (blockType) {
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return Block::SelectorType::SAPLING;
		break;
	case Block::BlockType::Sugar_Cane:
		return Block::SelectorType::BILBOARD_FULL_L;
		break;
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
		return Block::SelectorType::FLOWER;
		break;
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		return Block::SelectorType::MUSHROOM;
		break;
	default:
		return Block::SelectorType::BLOCK;
		break;
	}
}

void Block::SetBlockType(Block::BlockType type) noexcept
{
	blockType = type;
}

const std::array<std::array<float, 2>, 6>& Block::GetTexCoords() const noexcept
{
	return BlockTextures[static_cast<int>(blockType)];
}

AABB Block::GetAABB() const noexcept
{
	switch (GetSelectorType()) {
	case Block::SelectorType::BILBOARD_FULL_L:
		return AABB({ 0.8f, 2.0f, 0.8f });
	case Block::SelectorType::SAPLING:
		return AABB({ 0.6f, 1.2f, 0.6f });
	case Block::SelectorType::FLOWER:
		return AABB({ 0.4f, 1.4f, 0.4f });
	case Block::SelectorType::MUSHROOM:
		return AABB({ 0.4f, 0.75f, 0.4f });
	default:
		return AABB({ 1.0f, 2.0f, 1.0f });
	}
}

bool Block::NeedsSeperateDrawCall() const noexcept
{
	switch (blockType)
	{
	case Block::BlockType::Water:
		return true;
	default:
		return false;
	}
}

bool Block::IsLiquid() const noexcept
{
	switch (blockType)
	{
	case Block::BlockType::Water:
	case Block::BlockType::Lava:
		return true;
	default:
		return false;
	}
}
