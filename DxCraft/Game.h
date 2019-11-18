#pragma once
#include "WinConfig.h"
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "ThreadSettings.h"
#include <memory>
#include "WorldManager.h"

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
	Window wnd;
	Timer timer;
	Camera cam;
	bool exit = false;
	bool showCursor = true;
	float skyIntesity = 0.5f;
	float cameraSpeed = 15.0f;
	int chunkZ = 0;
	WorldManager wManager;
};