#pragma once
#include "Graphics.h"
#include "RenderData.h"
#include "Camera.h"
#include "Ray.h"
#include "Timer.h"
#include "WorldManager.h"
#include "Position.h"
#include "Crosshair.h"
#include "BlockSelector.h"
#include <DirectXMath.h>
#include "XM_Structs.h"

class Player
{
public:
	Player(Graphics& gfx, WorldManager& wManager);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;
	void SetSpeed(float velocity);
	float GetVelocity();
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRigth();
	void MoveUp(bool external = false);
	void MoveDown(bool external = false);
	void RotateCamera(float dx, float dy);
	void RightClickEvent();
	void LeftClickEvent();
	void LoopThenDraw();
	void ChangeBlock(bool decrement = false);
	Camera& GetCamera() noexcept;
	void ToggleFlying() noexcept;

private:
	void CastRay();
	void ResolveCollision(DirectX::XMFLOAT3 delta);
	int blockIndex = 1;
	std::string blockName = "Stone";
	Block::BlockType type = Block::BlockType::Stone;
	Graphics& gfx;
	WorldManager& wManager;
	Camera cam;
	BlockSelector blockSelector;
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};
	RenderData crosshair;
	bool found = false;
	DirectX::XMFLOAT3 hitBlock;
	DirectX::XMFLOAT3 previousHitBlock;
	DirectX::XMFLOAT3 momentum{0.0f, 0.0f, 0.0f};
	Position hitBlockPos;
	Ray cameraRay;
	float speed = 7.0f;
	Timer placeTimer;
	Timer destroyTimer;
	Timer jumpTimer;
	bool collision = true;
	bool flying = false;
	bool falling = false;

	float fallVelocity;
	float moveVelocity = 0.0f;

	const float baseSpeed = speed;

	const float velocityIncreaseConstant = 0.025f;
	float velocityMaxBound = 1.0f;
	const float velocityMinBound = 0.25f;
	const float collisionMaxBoud = 0.7f;

	float jumpVelocity = 0.0f;
	float jumpDistance = 10.5f;

	float jumpSpeedModifier = 7.5f;
	float fallSpeedModifier = 2.5f;

	float fallMinBound = 4.0f;

	bool jumping = false;
	
	const float moveConstant = 0.0125f;

	const float jumpFallConstant = 0.02f;

	const float flyingSpeedModifier = 4.0f;

	const float blockFace = 0.05f;

	const float diagonalBlockFace = blockFace;

	const float fallingDecreaseConstant = 0.2f;

	const float jumpingDecreaseConstant = 0.75f;
};
