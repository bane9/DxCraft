#include "EventManager.h"
#include <array>



using PlaceSignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition,
	const Position & PlaceDirection, Block::BlockType BlockType);
#define PLACE_PARAMATERS WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType

std::array<std::function<PlaceSignature>, 4> PlaceEvent = {
	[](PLACE_PARAMATERS) { //Default
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if(result) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Sugar cane
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass &&
			block->GetBlockType() != Block::BlockType::Sugar_Cane)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Flowers/Saplings
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
	[](PLACE_PARAMATERS) { //Mushrooms
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass
			&& block->GetBlockType() != Block::BlockType::Stone)) return false;
		bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
		if (result) blockEvt.CreateSurroundingUpdates(BlockPosition);
		return result;
	},
};
#undef PLACE_PARAMATERS



using DestroySignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, 
	const Position& BlockPosition);
#define DESTROY_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const Position& BlockPosition

std::array<std::function<DestroySignature>, 2> DestroyEvent = {
		[](DESTROY_PARAMATERS) { //Default
			bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
			if (result) blockEvt.CreateSurroundingUpdates(BlockPosition);
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
					Timer{}.GetTime() * 1000.0f,
					0.1f / 1000.0f
				};
				blockEvt.AddEvent(evt);
			}
			bool result = wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
			if (result) blockEvt.CreateSurroundingUpdates(BlockPosition);
			return result;
		},
};
#undef DESTROY_PARAMATERS


using UpdateSignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, const BlockEventManager::Event& evt);
#define UPDATE_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const BlockEventManager::Event& evt

std::array<std::function<UpdateSignature>, 2> UpdateEvent = {
	[](UPDATE_PARAMATERS) { //Default
		return true;
	},
	[](UPDATE_PARAMATERS) { //Stuff that cannot float
		Position pos = evt.blockPosition;
		auto block = wManager.GetBlock(pos.x, pos.y - 1, pos.z);
		if (block != nullptr || block->GetBlockType() == Block::BlockType::Air) {
			auto currentBlock = wManager.GetBlock(pos.x, pos.y, pos.z);
			if (currentBlock != nullptr) {
				BlockEventManager::Event evt1 = {
					{pos.x, pos.y, pos.z},
					{0, 0, 0},
					block->GetBlockType(),
					BlockEventManager::Event::EventType::REMOVE_BLOCK,
					Timer{}.GetTime() * 1000.0f,
					0.1f / 1000.0f
				};
				blockEvt.AddEvent(evt1);
			}
			wManager.ModifyBlock(pos.x, pos.y, pos.z);
		}
		return true;
	}
};
#undef UPDATE_PARAMATERS


BlockEventManager::BlockEventManager(WorldManager& wManager)
	: wManager(wManager)
{
}

bool BlockEventManager::PlaceBlock(const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType)
{
	switch (BlockType)
	{
	case Block::BlockType::Sugar_Cane:
		return PlaceEvent[1](wManager, *this, BlockPosition, PlaceDirection, BlockType);
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return PlaceEvent[2](wManager, *this, BlockPosition, PlaceDirection, BlockType);
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		return PlaceEvent[3](wManager, *this, BlockPosition, PlaceDirection, BlockType);
	default:
		return PlaceEvent[0](wManager, *this, BlockPosition, PlaceDirection, BlockType);
	}
}

bool BlockEventManager::RemoveBlock(const Position& BlockPosition)
{
	auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
	if (block == nullptr) return false;
	switch (block->GetBlockType())
	{
	case Block::BlockType::Sugar_Cane:
		return DestroyEvent[1](wManager, *this, BlockPosition);
	default:
		return DestroyEvent[0](wManager, *this, BlockPosition);
	}
}

void BlockEventManager::Loop(float FrametimeCompesator)
{
	auto tempEvents = Events;
	Events.clear();
	for (auto it = tempEvents.begin(); it < tempEvents.end(); ++it) {
		const auto& evt = *it;
		if (Timer{}.GetTime() * 1000.0f - evt.StartTime < (evt.DelayUntilUpdate * FrametimeCompesator)) {
			Events.push_back(*it);
			continue;
		}
		if (evt.eventType == Event::EventType::REMOVE_BLOCK) {
			RemoveBlock(evt.blockPosition);
		}
		else if (evt.eventType == Event::EventType::PLACE_BLOCK) {
			PlaceBlock(evt.blockPosition, evt.placeDirection, evt.blockType);
		}
		else if (evt.eventType == Event::EventType::UPDATE_BLOCK) {
			bool result;
			auto block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z);
			if (block == nullptr) continue;
			auto asd = block->GetBlockType();
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
				default:
					result = UpdateEvent[0](wManager, *this, evt);
					break;
			}
			if (!result) {
				Events.push_back(*it);
				continue;
			}
		}
		if ((it = tempEvents.erase(it)) == tempEvents.end()) break;
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
		Timer{}.GetTime() * 1000.0f,
		0.1f / 1000.0f
	};

	AddEvent(evt);
}
