#include "EventManager.h"
#include <array>



using PlaceSignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition,
	const Position & PlaceDirection, Block::BlockType BlockType);
#define PLACE_PARAMATERS WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType

std::array<std::function<PlaceSignature>, 2> PlaceEvent = {
	[](PLACE_PARAMATERS) {
		return wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
	},
	[](PLACE_PARAMATERS) {
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass &&
			block->GetBlockType() != Block::BlockType::Sugar_Cane)) return false;
		return wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
	},
};
#undef PLACE_PARAMATERS



using DestroySignature = bool(WorldManager& wManager, BlockEventManager& blockEvt, 
	const Position& BlockPosition);
#define DESTROY_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const Position& BlockPosition

std::array<std::function<DestroySignature>, 2> DestroyEvent = {
		[](DESTROY_PARAMATERS) {
			return wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
		},
		[](DESTROY_PARAMATERS) {
			auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y + 1, BlockPosition.z);
			if (block != nullptr && block->GetBlockType() == Block::BlockType::Sugar_Cane) {
				BlockEventManager::Event evt = {
					{BlockPosition.x, BlockPosition.y + 1, BlockPosition.z},
					{0, 0, 0},
					Block::BlockType::Sugar_Cane,
					BlockEventManager::Event::EventType::REMOVE_BLOCK,
					Timer{}.GetTime() * 1000000.0f,
					0.1f
				};
				blockEvt.AddEvent(evt);
			}
			return wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
		},
};
#undef DESTROY_PARAMATERS


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

void BlockEventManager::Loop()
{
	auto tempEvents = Events;
	Events.clear();
	for (auto it = tempEvents.begin(); it < tempEvents.end(); ++it) {
		const auto& evt = *it;
		if (Timer{}.GetTime() * 1000000.0f - evt.StartTime < evt.DelayUntilUpdate) {
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

		}
		if ((it = tempEvents.erase(it)) == tempEvents.end()) break;
	}
}

void BlockEventManager::AddEvent(const Event& event)
{
	Events.push_back(event);
}

