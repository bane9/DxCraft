#pragma once

enum class BlockType {
	Air,
	Grass,
	Dirt,
	Stone,
	Bedrock
};

struct Block
{
	int x, y, z;
	BlockType type;
};