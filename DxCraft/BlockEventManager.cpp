#include "BlockEventManager.h"
#include <array>
#include <algorithm>

#define WATER_SPREAD_RATE 500.0f / 1000.0f

#define PLACE_PARAMATERS WorldManager& wManager, BlockEventManager& blockEvt, const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType, BlockEventManager::Event evt
using PlaceSignature = bool(PLACE_PARAMATERS);

inline bool HandleLiquid(WorldManager& wManager, BlockEventManager& blockEvt, BlockEventManager::Event& evt) {
	if (evt.water_level < 1) return true;
	Block* block = nullptr;
	
	bool stop = false;

	struct {
		bool spread = false;
		int spread_amount = 1;
		bool dead_end = false;
	} forward, backward, left, right;
#define CBLKAOW(x) (x->GetBlockType() == Block::BlockType::Air || x->GetBlockType() == Block::BlockType::Water)
	if (true) {
		for (int i = 1; i < evt.water_level; i++) {
			if (!forward.dead_end && !forward.spread) {
				block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z + i);
				if (block != nullptr && CBLKAOW(block)) {
					block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y - 1, evt.blockPosition.z + i);
					if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
						block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z + i);
						stop = true;
						forward.spread = true;
					}
					forward.spread_amount++;
				}
				else forward.dead_end = true;
			}

			if (!backward.dead_end && !backward.spread) {
				block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z - i);
				if (block != nullptr && CBLKAOW(block)) {
					block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y - 1, evt.blockPosition.z - i);
					if (block != nullptr && CBLKAOW(block)) {
						stop = true;
						backward.spread = true;
					}
					backward.spread_amount++;
				}
				else backward.dead_end = true;
			}

			if (!right.dead_end && !right.spread) {
				block = wManager.GetBlock(evt.blockPosition.x + i, evt.blockPosition.y, evt.blockPosition.z);
				if (block != nullptr && CBLKAOW(block)) {
					block = wManager.GetBlock(evt.blockPosition.x + i, evt.blockPosition.y - 1, evt.blockPosition.z);
					if (block != nullptr && CBLKAOW(block)) {
						stop = true;
						right.spread = true;
					}
					right.spread_amount++;
				}
				else forward.dead_end = true;
			}

			if (!left.dead_end && !left.spread) {
				block = wManager.GetBlock(evt.blockPosition.x - i, evt.blockPosition.y, evt.blockPosition.z);
				if (block != nullptr && CBLKAOW(block)) {
					block = wManager.GetBlock(evt.blockPosition.x - i, evt.blockPosition.y - 1, evt.blockPosition.z);
					if (block != nullptr && CBLKAOW(block)) {
						stop = true;
						left.spread = true;
					}
					left.spread_amount++;
				}
				else left.dead_end = true;
			}

			if (stop) break;
		}

		if (forward.spread) {
			for (int i = 1; i < forward.spread_amount; i++) {
				BlockEventManager::Event evt1 = evt;
				evt1.blockPosition.z += i;
				evt1.water_level = forward.spread_amount - i;
				evt1.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
				evt1.StartTime = blockEvt.EventTimer.GetTime();
				evt1.DelayUntilUpdate = WATER_SPREAD_RATE * i;
				evt1.UpdateDepth = i == forward.spread_amount - 1 ? 0 : 1;
				blockEvt.AddEvent(evt1);
			}
		}
		if (backward.spread) {
			for (int i = 1; i < backward.spread_amount; i++) {
				BlockEventManager::Event evt1 = evt;
				evt1.blockPosition.z -= i;
				evt1.water_level = backward.spread_amount - i;
				evt1.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
				evt1.StartTime = blockEvt.EventTimer.GetTime();
				evt1.DelayUntilUpdate = WATER_SPREAD_RATE * i;
				evt1.UpdateDepth = i == backward.spread_amount - 1 ? 0 : 1;
				blockEvt.AddEvent(evt1);
			}
		}
		if (right.spread) {
			for (int i = 1; i < right.spread_amount; i++) {
				BlockEventManager::Event evt1 = evt;
				evt1.blockPosition.x += i;
				evt1.water_level = right.spread_amount - i;
				evt1.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
				evt1.StartTime = blockEvt.EventTimer.GetTime();
				evt1.DelayUntilUpdate = WATER_SPREAD_RATE * i;
				evt1.UpdateDepth = i == right.spread_amount - 1 ? 0 : 1;
				blockEvt.AddEvent(evt1);
			}
		}
		if (left.spread) {
			for (int i = 1; i < left.spread_amount; i++) {
				BlockEventManager::Event evt1 = evt;
				evt1.blockPosition.x -= i;
				evt1.water_level = left.spread_amount - i;
				evt1.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
				evt1.StartTime = blockEvt.EventTimer.GetTime();
				evt1.DelayUntilUpdate = WATER_SPREAD_RATE * i;
				evt1.UpdateDepth = i == left.spread_amount - 1 ? 0 : 1;
				blockEvt.AddEvent(evt1);
			}
		}
		if (stop) return true;
	}

	block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y - 1, evt.blockPosition.z);
	if (block != nullptr && CBLKAOW(block)) {
		BlockEventManager::Event evt1 = evt;
		evt1.blockPosition.y--;
		evt1.water_level = 6;
		evt1.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
		evt1.StartTime = blockEvt.EventTimer.GetTime();
		evt1.UpdateDepth = 0;
		evt1.DelayUntilUpdate = WATER_SPREAD_RATE;
		blockEvt.AddEvent(evt1);
		if (evt.water_level < 7) return true;
	}

	

	block = wManager.GetBlock(evt.blockPosition.x + 1, evt.blockPosition.y, evt.blockPosition.z);
	if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
		BlockEventManager::Event newEvt = evt;
		newEvt.blockPosition.x++;
		newEvt.water_level--;
		newEvt.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
		newEvt.StartTime = blockEvt.EventTimer.GetTime();
		newEvt.DelayUntilUpdate = WATER_SPREAD_RATE;
		blockEvt.AddEvent(newEvt);
	}


	block = wManager.GetBlock(evt.blockPosition.x - 1, evt.blockPosition.y, evt.blockPosition.z);
	if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
		BlockEventManager::Event newEvt = evt;
		newEvt.blockPosition.x--;
		newEvt.water_level--;
		newEvt.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
		newEvt.StartTime = blockEvt.EventTimer.GetTime();
		newEvt.DelayUntilUpdate = WATER_SPREAD_RATE;
		blockEvt.AddEvent(newEvt);
	}

	block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z + 1);
	if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
		BlockEventManager::Event newEvt = evt;
		newEvt.blockPosition.z++;
		newEvt.water_level--;
		newEvt.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
		newEvt.StartTime = blockEvt.EventTimer.GetTime();
		newEvt.DelayUntilUpdate = WATER_SPREAD_RATE;
		blockEvt.AddEvent(newEvt);
	}
	
	block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z - 1);
	if (block != nullptr && block->GetBlockType() == Block::BlockType::Air) {
		BlockEventManager::Event newEvt = evt;
		newEvt.blockPosition.z--;
		newEvt.water_level--;
		newEvt.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
		newEvt.StartTime = blockEvt.EventTimer.GetTime();
		newEvt.DelayUntilUpdate = WATER_SPREAD_RATE;
		blockEvt.AddEvent(newEvt);
	}
}

std::array<std::function<PlaceSignature>, 6> PlaceEvent = {
	[](PLACE_PARAMATERS) { //Default
		bool result = wManager.ModifyBlock(BlockPosition, BlockType);
		if(result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
		return result;
	},
	[](PLACE_PARAMATERS) { //Sugar cane
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass &&
			block->GetBlockType() != Block::BlockType::Sugar_Cane)) return false;
		bool result = wManager.ModifyBlock(BlockPosition, BlockType);
		if (result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
		return result;
	},
	[](PLACE_PARAMATERS) { //Flowers/Saplings
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass)) return false;
		bool result = wManager.ModifyBlock(BlockPosition, BlockType);
		if (result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
		return result;
	},
	[](PLACE_PARAMATERS) { //Mushrooms
		auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y - 1, BlockPosition.z);
		if (block == nullptr || (block->GetBlockType() != Block::BlockType::Grass
			&& block->GetBlockType() != Block::BlockType::Stone)) return false;
		bool result = wManager.ModifyBlock(BlockPosition, BlockType);
		if (result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
		return result;
	},
	[](PLACE_PARAMATERS) { //No spread
		return wManager.ModifyBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z, BlockType);
	},
	[](PLACE_PARAMATERS) { //Liquid
		if (evt.water_level < 1) return true;
		wManager.ModifyBlock(evt.blockPosition, Block::BlockType::Water);
		auto block = wManager.GetBlock(evt.blockPosition);
		block->liquidInfo.level = evt.water_level;
		if (evt.UpdateDepth > 0) return true;
		bool result = HandleLiquid(wManager, blockEvt, evt);
		//blockEvt.CreateSurroundingUpdates(evt.blockPosition, 1);
		return result;
	},
};
#undef PLACE_PARAMATERS



#define DESTROY_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, const Position& BlockPosition, BlockEventManager::Event evt
using DestroySignature = bool(DESTROY_PARAMATERS);

std::array<std::function<DestroySignature>, 3> DestroyEvent = {
		[](DESTROY_PARAMATERS) { //Default
			bool result = wManager.ModifyBlock(BlockPosition);
			if (result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
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
			bool result = wManager.ModifyBlock(BlockPosition);
			if (result && evt.UpdateDepth == 0) blockEvt.CreateSurroundingUpdates(BlockPosition, 1);
			return result;
		},
		[](DESTROY_PARAMATERS) { //No spread
			return wManager.ModifyBlock(BlockPosition);
		}
};
#undef DESTROY_PARAMATERS

#define UPDATE_PARAMATERS WorldManager& wManager,  BlockEventManager& blockEvt, BlockEventManager::Event& evt
using UpdateSignature = bool(UPDATE_PARAMATERS);

std::array<std::function<UpdateSignature>, 4> UpdateEvent = {
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
	},
	[](UPDATE_PARAMATERS) { //Liquid
		HandleLiquid(wManager, blockEvt, evt);
		return true;
	},
};
#undef UPDATE_PARAMATERS


BlockEventManager::BlockEventManager(WorldManager& wManager)
	: wManager(wManager), gen(rd()), DestroyDelayRand(100.0, 5000.0)
{
}

bool BlockEventManager::PlaceBlock(const Position& BlockPosition, const Position& PlaceDirection, 
	Block::BlockType BlockType, Event evt)
{
	switch (BlockType)
	{
	case Block::BlockType::Sugar_Cane:
		return PlaceEvent[1](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	case Block::BlockType::Dandelion:
	case Block::BlockType::Poppy:
	case Block::BlockType::Birch_Sapling:
	case Block::BlockType::Oak_Sapling:
	case Block::BlockType::Dark_Oak_Sapling:
		return PlaceEvent[2](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	case Block::BlockType::Brown_Mushroom:
	case Block::BlockType::Red_Mushroom:
		return PlaceEvent[3](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	case Block::BlockType::Leaves:
	case Block::BlockType::Oak_Wood:
		return PlaceEvent[4](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	case Block::BlockType::Water:
	{
		//auto block = wManager.GetBlock(evt.blockPosition.x, evt.blockPosition.y, evt.blockPosition.z);
		//if (block == nullptr || block->GetBlockType() != Block::BlockType::Air) return false;
		return PlaceEvent[5](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	}
	default:
		return PlaceEvent[0](wManager, *this, BlockPosition, PlaceDirection, BlockType, evt);
	}
}

bool BlockEventManager::RemoveBlock(const Position& BlockPosition, Event evt)
{
	auto block = wManager.GetBlock(BlockPosition.x, BlockPosition.y, BlockPosition.z);
	if (block == nullptr) return false;
	switch (block->GetBlockType())
	{
	case Block::BlockType::Sugar_Cane:
		return DestroyEvent[1](wManager, *this, BlockPosition, evt);
	case Block::BlockType::Leaves:
	case Block::BlockType::Oak_Wood:
		return DestroyEvent[2](wManager, *this, BlockPosition, evt);
	default:
		return DestroyEvent[0](wManager, *this, BlockPosition, evt);
	}
}

void BlockEventManager::Loop()
{
	auto tempEvents = Events;
	Events.clear();
	for (auto& evt : tempEvents) {
		if (EventTimer.GetTime() - evt.StartTime < evt.DelayUntilUpdate) {
			Events.push_back(evt);
		}
		else if (evt.eventType == Event::EventType::REMOVE_BLOCK) {
			RemoveBlock(evt.blockPosition, evt);
		}
		else if (evt.eventType == Event::EventType::PLACE_BLOCK) {
			PlaceBlock(evt.blockPosition, evt.placeDirection, evt.blockType, evt);
		}
		else if (evt.eventType == Event::EventType::UPDATE_BLOCK) {
			auto block = wManager.GetBlock(evt.blockPosition);
			if (block == nullptr) continue;
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
			case Block::BlockType::Water:
			{
				auto block = wManager.GetBlock(evt.blockPosition);
				evt.water_level = block->liquidInfo.level;
				evt.blockType = block->GetBlockType();
				//result = UpdateEvent[3](wManager, *this, evt);
				break;
			}
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

void BlockEventManager::CreateSurroundingUpdates(const Position& BlockPosition, int updateDepth)
{
	Event evt{
		BlockPosition,
		{0, 0, 0},
		Block::BlockType::None,
		BlockEventManager::Event::EventType::UPDATE_BLOCK,
		EventTimer.GetTime(),
		75.0f / 1000.0f,
		updateDepth
	};

	--evt.blockPosition.x;
	AddEvent(evt);
	++evt.blockPosition.x;

	++evt.blockPosition.x;
	AddEvent(evt);
	--evt.blockPosition.x;

	--evt.blockPosition.y;
	AddEvent(evt);
	++evt.blockPosition.y;

	++evt.blockPosition.y;
	AddEvent(evt);
	--evt.blockPosition.y;

	--evt.blockPosition.z;
	AddEvent(evt);
	++evt.blockPosition.z;

	++evt.blockPosition.z;
	AddEvent(evt);
}

