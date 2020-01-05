#pragma once
#include "Block.h"
#include "Timer.h"
#include "Position.h"
#include <vector>
#include "WorldManager.h"
#include <functional>

class BlockEventManager
{
	BlockEventManager(const BlockEventManager&) = delete;
	BlockEventManager& operator=(const BlockEventManager&) = delete;
public:
	BlockEventManager(WorldManager& wManager);
	bool PlaceBlock(const Position& BlockPosition, const Position& PlaceDirection, Block::BlockType BlockType);
	bool RemoveBlock(const Position& BlockPosition);
	void Loop(float FrametimeCompesator);
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
	};
	void AddEvent(const Event& event);
	void CreateSurroundingUpdates(const Position& BlockPosition);
private:
	WorldManager& wManager;
	std::vector<Event> Events;
};

