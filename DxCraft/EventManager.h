#pragma once
#include "Block.h"
#include "Timer.h"
#include "Position.h"
#include <vector>
#include "WorldManager.h"
#include <functional>
#include <random>

class BlockEventManager
{
	friend class TreeGenerator;
	BlockEventManager(const BlockEventManager&) = delete;
	BlockEventManager& operator=(const BlockEventManager&) = delete;
public:
	struct Event {
		enum EventType {
			PLACE_BLOCK,
			REMOVE_BLOCK,
			UPDATE_BLOCK
		};
		Position blockPosition;
		Position placeDirection;
		Block::BlockType blockType;
		EventType eventType;
		float StartTime = 0.0f;
		float DelayUntilUpdate = 0.0f;
		int UpdateDepth = 0;
		struct DependantOnBlock {
			Position pos = { 0, 0, 0 };
			Block::BlockType type = Block::BlockType::None;
		};
		DependantOnBlock dependantOnBlock;
		int water_level = 0;
	};
	BlockEventManager(WorldManager& wManager);
	bool PlaceBlock(const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType, Event evt = {});
	bool RemoveBlock(const Position& BlockPosition, Event evt = {});
	void Loop();
	void AddEvent(const Event& event);
	void CreateSurroundingUpdates(const Position& BlockPosition, int updateDepth = 0);
	Timer EventTimer;
	std::mt19937 gen;
	std::uniform_real_distribution<> DestroyDelayRand;
private:
	WorldManager& wManager;
	std::vector<Event> Events;
	std::random_device rd;
};

