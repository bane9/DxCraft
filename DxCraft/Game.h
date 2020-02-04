#pragma once
#include "WinConfig.h"
#include "Window.h"
#include "ThreadSettings.h"
#include "WorldManager.h"
#include "Player.h"
#include "RenderData.h"
#include "FastNoise.h"
#include <mutex>
#include "ConcurrentQueue.h"
#include <utility>

class Game
{
public:
	Game(size_t width, size_t height);
	Game(Game&) = delete;
	Game& operator=(Game&) = delete;
	~Game();
	int Start();
private:
	void DoFrame();
	void MakeChunkThread();
	Window wnd;
	bool exit = false;
	bool showCursor = true;
	float skyIntesity = 0.5f;
	WorldManager wManager;
	Player player;
	Timer jumpTimer;
	Timer sprintTimer;
	int area = 55 * Chunk::ChunkSize;
	ConcurentQueue<Position> positionQueue;
};
