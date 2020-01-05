#include "TreeGenerator.h"
#include <array>
#include <utility>

std::array<std::array<std::pair<Position, Block::BlockType>, 20>, 1> Trees = {
	{
		std::make_pair(Position(0, 1, 0), Block::BlockType::Oak_Wood),
		std::make_pair(Position(0, 2, 0), Block::BlockType::Oak_Wood),
		std::make_pair(Position(0, 3, 0), Block::BlockType::Oak_Wood),
		std::make_pair(Position(0, 4, 0), Block::BlockType::Oak_Wood),

		std::make_pair(Position(1, 3, 0), Block::BlockType::Leaves),
		std::make_pair(Position(0, 3, 1), Block::BlockType::Leaves),
		std::make_pair(Position(1, 3, 1), Block::BlockType::Leaves),
		std::make_pair(Position(1, 4, 0), Block::BlockType::Leaves),
		std::make_pair(Position(0, 4, 1), Block::BlockType::Leaves),
		std::make_pair(Position(1, 4, 1), Block::BlockType::Leaves),

		std::make_pair(Position(-1, 3, 0), Block::BlockType::Leaves),
		std::make_pair(Position(0, 3, -1), Block::BlockType::Leaves),
		std::make_pair(Position(-1, 3, -1), Block::BlockType::Leaves),
		std::make_pair(Position(-1, 4, 0), Block::BlockType::Leaves),
		std::make_pair(Position(0, 4, -1), Block::BlockType::Leaves),
		std::make_pair(Position(-1, 4, -1), Block::BlockType::Leaves),

		std::make_pair(Position(1, 3, -1), Block::BlockType::Leaves),
		std::make_pair(Position(-1, 3, 1), Block::BlockType::Leaves),
		std::make_pair(Position(-1, 4, 1), Block::BlockType::Leaves),

		std::make_pair(Position(0, 5, 0), Block::BlockType::Leaves),
	}
};

bool TreeGenerator::GenerateTree(BlockEventManager& blockMgr, const Position& pos, TreeGenerator::TreeType type)
{
	for (auto& pair : Trees[0]) {
		Position blockPos = pos + pair.first;
		auto block = blockMgr.wManager.GetBlock(blockPos.x, blockPos.y, blockPos.z);
		if (block != nullptr && block->GetBlockType() != Block::BlockType::Air) return false;
	}
	blockMgr.wManager.ModifyBlock(pos.x, pos.y, pos.z, Block::BlockType::Oak_Wood);
	for (auto& pair : Trees[0]) {
		Position blockPos = pos + pair.first;
		blockMgr.PlaceBlock(blockPos, {0, 0, 0}, pair.second);
		if (pair.second == Block::BlockType::Leaves) {
			BlockEventManager::Event evt =
			{
				blockPos,
				{0, 0, 0},
				Block::BlockType::Leaves,
				BlockEventManager::Event::EventType::UPDATE_BLOCK,
				0.0f, 0.0f, 0,
				{pos + Position(0, 4, 0), Block::BlockType::Oak_Wood}
			};
			blockMgr.AddEvent(evt);
		}
	}
	return true;
}
