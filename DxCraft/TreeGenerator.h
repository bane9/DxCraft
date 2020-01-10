#pragma once
#include "Block.h"
#include "BlockEventManager.h"
#include "Position.h"

class TreeGenerator
{
	TreeGenerator() = delete;
public:
	enum TreeType {
		Oak,
		Dark_Oak,
		Birch
	};
	static bool GenerateTree(BlockEventManager& blockMgr, const Position& pos, TreeType type = TreeType::Oak);

};

