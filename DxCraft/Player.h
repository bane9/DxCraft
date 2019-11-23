#pragma once
#include "Graphics.h"
#include "RendererData.h"
#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"
#include "Timer.h"
#include "WorldManager.h"
#include "Position.h"
#include "Crosshair.h"
#include "BlockSelector.h"
#include <DirectXMath.h>

class Player
{
public:
	Player(Graphics& gfx, WorldManager& wManager);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	void SetVelocity(float velocity);
	float GetVelocity();
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRigth();
	void MoveUp();
	void MoveDown(bool external = false);
	void CastRay();
	void RotateCamera(float dx, float dy);
	void RightClickEvent();
	void LeftClickEvent();
	void Draw();

private:
	void ResolveCollision(DirectX::XMFLOAT3 delta);
	Graphics& gfx;
	WorldManager& wManager;
	Camera cam;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	RendererData<DirectX::XMFLOAT3, DirectX::XMMATRIX> crosshair;
	RendererData<DirectX::XMFLOAT3, Transforms> blockSelector;
	bool found = false;
	DirectX::XMFLOAT3 hitBlock;
	DirectX::XMFLOAT3 previousHitBlock;
	Position hitBlockPos;
	Ray cameraRay;
	float velocity = 7.5f;
	Timer placeTimer;
	Timer destroyTimer;
	Timer moveTimer;
	Timer fallTimer;
	float dt = 0.01f;
	bool collision = true;
	bool flying = false;
	bool falling = false;
};

