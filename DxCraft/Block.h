#pragma once
#include <array>

enum class BlockType {
	Air,
	Stone,
	Grass,
	Dirt,
	Wooden_Plank,
	Bedrock,
	/*Stone_Slab,
	Brick,
	TNT,
	Cobweb,
	Rose,
	Dandelion,
	Sapling,
	CobbleStone,
	Bedrock,
	Sand,
	Gravel,
	Wood,*/


	block_count
};

constexpr std::array<std::array<std::array<float, 2>, 6>, static_cast<int>(BlockType::block_count)> BlockFaces
{{
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
	}}

}};

struct Block
{
	int x, y, z;
	BlockType type;
};
