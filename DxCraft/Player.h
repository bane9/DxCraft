#pragma once
#include "Graphics.h"
#include "RendererData.h"
#include "Renderer.h"
#include "Camera.h"
#include "Ray.h"
#include "AABB.h"
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
	void MoveDown();
	void CastRay();
	void RotateCamera(float dx, float dy);
	void RightClickEvent();
	void LeftClickEvent();
	void Draw();

private:
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
	AABB playerBox;
	float velocity = 15.0f;
	Timer placeTimer;
	Timer destroyTimer;
	Timer moveTimer;
	float dt = 0.01f;

};

