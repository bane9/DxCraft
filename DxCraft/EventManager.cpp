#include "EventManager.h"
#include <array>


using PlaceSignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition,
	const Position & PlaceDirection, Block::BlockType BlockType, int evtDepth);
#define PLACE_PARAMATERS WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType, int evtDepth

std::array<std::function<PlaceSignature>, 5> PlaceEvent = {
	[](PLACE_PARAMATERS) { //Default
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if(result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Sugar cane
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass &&
			block->GetBlockType() != Block::BlockType::Sugar_Cane)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Flowers/Saplings
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Mushrooms
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass
			&& block->GetBlockType() != Block::BlockType::Stone)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //No update
		wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		return true;
	},
};
#undef PLACE_PARAMATERS



using DestroySignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, 
	const Position& BlockPosition, int evtDepth);
#define DESTROY_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const Position& BlockPosition, int evtDepth

std::array<std::function<DestroySignature>, 3> DestroyEvent = {
		[](DESTROY_PARAMATERS) { //Default
			bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
			if (result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
			return result;
		},
		[](DESTROY_PARAMATERS) { //Sugar cane
			auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y + 1, BlockPosition.z);
			if (block != nullptr && block->GetBlockType() == Block::BlockType::Sugar_Cane) {
				BlockEventManager::Event evt = {
					{BlockPosition.x, BlockPosition.y + 1, BlockPosition.z},
					{0, 0, 0},
					Block::BlockType::Sugar_Cane,
					BlockEventManager::Event::EventType::REMOVE_BLOCK,
					blockEvt.EventTimer.GetTime(),
					75.0 / 1000.0f
				};
				blockEvt.AddEvent(evt);
			}
			bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
			if (result && evtDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition);
			return result;
		},
		[](DESTROY_PARAMATERS) { //No spread
			wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
			return true;
		}
};
#undef DESTROY_PARAMATERS


using UpdateSignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, const BlockEventManager::Event& evt);
#define UPDATE_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const BlockEventManager::Event& evt

std::array<std::function<UpdateSignature>, 3> UpdateEvent = {
	[](UPDATE_PARAMATERS) { //Default
		return true;
	},
	[](UPDATE_PARAMATERS) { //Stuff that cannot float
		Position pos = evt.blockPosition;
		auto block = wManager.GetBlock(pos.x, pos.y - 1, pos.z);
		if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
			auto currentBlock = wManager.GetBlock(pos.x, pos.y, pos.z);
			if (currentBlock != nullptr) {
				BlockEventManager::Event evt1 = {
					{pos.x, pos.y, pos.z},
					{0, 0, 0},
					block->GetBlockType(),
					BlockEventManager::Event::EventType::REMOVE_BLOCK,
					blockEvt.EventTimer.GetTime(),
					75.0f / 1000.0f
				};
				blockEvt.AddEvent(evt1);
			}
			wManager.ModifyBlock(pos.x, pos.y, pos.z);
		}
		return true;
	},
	[](UPDATE_PARAMATERS) { //Leaves
		auto block = wManager.GetBlock(evt.dependantOnBlock.pos.x, evt.dependantOnBlock.pos.y, evt.dependantOnBlock.pos.z);
		if (block == nullptr || block->GetBlockType() != evt.dependantOnBlock.type) {
			BlockEventManager::Event evt1 = {
					evt.blockPosition,
					{0, 0, 0},
					Block::BlockType::Leaves,
					BlockEventManager::Event::EventType::REMOVE_BLOCK,
					blockEvt.EventTimer.GetTime(),
					static_cast<float>(blockEvt.DestroyDelayRand(blockEvt.gen)) / 1000.0f
			};
			blockEvt.AddEvent(evt1);
			return true;
		}
		return false;
	}
};
#undef UPDATE_PARAMATERS


BlockEventManager::BlockEventManager(WorldManager& wManager)
	: wManager(wManager), gen(rd()), DestroyDelayRand(100.0, 5000.0)
{
}

bool BlockEventManager::PlaceBlock(const Position& BlockPosition, const Position& PlaceDirection, 
	Block::BlockType BlockType, int evtDepth)
{
	switch (BlockType)
	{
	case Block::BlockType::Sugar_Cane:
		return PlaceEvent[1](wManager, *this, BlockPosition, PlaceDirection, BlockType, evtDepth);
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return PlaceEvent[2](wManager, *this, BlockPosition, PlaceDirection, BlockType, evtDepth);
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		return PlaceEvent[3](wManager, *this, BlockPosition, PlaceDirection, BlockType, evtDepth);
	case Block::BlockType::Leaves:
	case Block::BlockType::Oak_Wood:
		return PlaceEvent[4](wManager, *this, BlockPosition, PlaceDirection, BlockType, evtDepth);
	default:
		return PlaceEvent[0](wManager, *this, BlockPosition, PlaceDirection, BlockType, evtDepth);
	}
}

bool BlockEventManager::RemoveBlock(const Position& BlockPosition, int evtDepth)
{
	auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
	if (block == nullptr) return false;
	switch (block->GetBlockType())
	{
	case Block::BlockType::Sugar_Cane:
		return DestroyEvent[1](wManager, *this, BlockPosition, evtDepth);
	case Block::BlockType::Leaves:
	case Block::BlockType::Oak_Wood:
		return DestroyEvent[2](wManager, *this, BlockPosition, evtDepth);
	default:
		return DestroyEvent[0](wManager, *this, BlockPosition, evtDepth);
	}
}

void BlockEventManager::Loop()
{
	auto tempEvents = Events;
	Events.clear();
	while (tempEvents.size() > 0) {
		auto evt = tempEvents.back();
		tempEvents.pop_back();
		if (EventTimer.GetTime() - evt.StartTime < evt.DelayUntilUpdate) {
			Events.push_back(evt);
		}
		else if (evt.eventType == Event::EventType::REMOVE_BLOCK) {
			RemoveBlock(evt.blockPosition, evt.UpdateDepth);
		}
		else if (evt.eventType == Event::EventType::PLACE_BLOCK) {
			PlaceBlock(evt.blockPosition, evt.placeDirection, evt.blockType, evt.UpdateDepth);
		}
		else if (evt.eventType == Event::EventType::UPDATE_BLOCK) {
			auto block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z);
			bool result;
			switch (block->GetBlockType()) {
			case Block::BlockType::Sugar_Cane:
			case Block::BlockType::Dandelion:
			case Block::BlockType::Poppy:
			case Block::BlockType::Birch_Sapling:
			case Block::BlockType::Oak_Sapling:
			case Block::BlockType::Dark_Oak_Sapling:
			case Block::BlockType::Brown_Mushroom:
			case Block::BlockType::Red_Mushroom:
				result = UpdateEvent[1](wManager, *this, evt);
				break;
			case Block::BlockType::Leaves:
				result = UpdateEvent[2](wManager, *this, evt);
				if (!result) Events.push_back(evt);
				break;
			default:
				result = UpdateEvent[0](wManager, *this, evt);
				break;
			}
		}
	}
}

void BlockEventManager::AddEvent(const Event& event)
{
	Events.push_back(event);
}

void BlockEventManager::CreateSurroundingUpdates(const Position& BlockPosition)
{
	Event evt{
		{BlockPosition.x, BlockPosition.y + 1, BlockPosition.z},
		{0, 0, 0},
		Block::BlockType::None,
		BlockEventManager::Event::EventType::UPDATE_BLOCK,
		EventTimer.GetTime(),
		75.0f / 1000.0f,
		1
	};

	AddEvent(evt);
}

