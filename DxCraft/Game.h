#pragma once
#include "WinConfig.h"
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "ThreadSettings.h"
#include <memory>
#include "WorldManager.h"
#include "Ray.h"
#include "RendererData.h"
#include "Crosshair.h"

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
	WorldManager wManager;
	Ray cameraRay;
	Timer placeTimer, destroyTimer;

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	RendererData<DirectX::XMFLOAT3> renderData;

	RendererData<DirectX::XMFLOAT3, Transforms> blockSelector;
};
