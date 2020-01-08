#pragma once
#include "WinConfig.h"
#include "Window.h"
#include "ThreadSettings.h"
#include "WorldManager.h"
#include "Player.h"
#include "RenderData.h"
#include "FastNoise.h"

class Game
{
public:
	Game(size_t width, size_t height);
	Game(Game&) = delete;
	Game& operator=(Game&) = delete;
	~Game();
	int start();
private:
	void doFrame();
	void MakeChunkThread();
	Window wnd;
	bool exit = false;
	bool showCursor = true;
	float skyIntesity = 0.5f;
	WorldManager wManager;
	Player player;
	Timer jumpTimer;
	Timer sprintTimer;
	RenderData test;
	FastNoise noise;
	int chunkCount = 0;
};
