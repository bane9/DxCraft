#pragma once
#include "WinConfig.h"
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "ThreadSettings.h"
#include <memory>

class Game
{
public:
	Game(size_t width, size_t heigth);
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
};