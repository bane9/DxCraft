#include "Player.h"

Player::Player(Graphics& gfx, WorldManager& wManager)
	: 
	gfx(gfx),
	wManager(wManager),
	crosshair(gfx, L"CrosshairVS.cso", L"CrossHairPS.cso", ied),
	blockSelector(gfx, L"SelectionVS.cso", L"SelectionPS.cso", ied),
	playerBox({1.0f, 1.7f, 1.0f}),
	hitBlockPos(-1, -1, -1)
{
	std::vector<DirectX::XMFLOAT3> tempVertices;
	std::copy(Crosshair::NearSide.first.begin(), Crosshair::NearSide.first.end(), std::back_inserter(tempVertices));

	std::vector<uint16_t> tempIndices;
	std::copy(Crosshair::NearSide.second.begin(), Crosshair::NearSide.second.end(), std::back_inserter(tempIndices));

	auto t = crosshair.UpdateVertexBuffer(tempVertices);
	auto t2 = crosshair.UpdateIndexBuffer(tempIndices);

	tempVertices.clear();
	std::copy(BlockSelector::Cube.first.begin(), BlockSelector::Cube.first.end(), std::back_inserter(tempVertices));

	tempIndices.clear();
	std::copy(BlockSelector::Cube.second.begin(), BlockSelector::Cube.second.end(), std::back_inserter(tempIndices));

	t = blockSelector.UpdateVertexBuffer(tempVertices);
	t2 = blockSelector.UpdateIndexBuffer(tempIndices);

	crosshair.UpdateConstantBuffer(DirectX::XMMatrixTranspose(gfx.getProjection()));

	cam.SetPos(0.0f, 25.0f, 0.0f);
}

void Player::SetVelocity(float velocity)
{
	this->velocity = velocity;
}

float Player::GetVelocity()
{
	return velocity;
}

void Player::MoveForward()
{
	cam.Translate({ 0.0f,0.0f,dt }, velocity);
}

void Player::MoveBackward()
{
	cam.Translate({ 0.0f,0.0f,-dt }, velocity);
}

void Player::MoveLeft()
{
	cam.Translate({ -dt,0.0f,0.0f }, velocity);
}

void Player::MoveRigth()
{
	cam.Translate({ dt,0.0f,0.0f }, velocity);
}

void Player::MoveUp()
{
	cam.Translate({ 0.0f,dt,0.0f }, velocity);
}

void Player::MoveDown()
{
	cam.Translate({ 0.0f,dt,0.0f }, velocity);
}

void Player::CastRay()
{
	cameraRay.SetPositionAndDirection(cam.GetPos(), cam.GetPitch(), cam.GetYaw());
	previousHitBlock = cameraRay.GetVector();
	hitBlock = previousHitBlock;
	found = false;

	hitBlockPos = { -1, -1, -1 };

	while (cameraRay.Next()) {
		auto block = wManager.GetBlock(round(hitBlock.x), round(hitBlock.y), round(hitBlock.z));
		if (block != nullptr && block->type != BlockType::Air) {
			hitBlockPos = { block->x,block->y, block->z };
			found = true;
			break;
		}
		previousHitBlock = std::move(hitBlock);
		hitBlock = cameraRay.GetVector();
	}
}

void Player::RotateCamera(float dx, float dy)
{
	cam.Rotate(dx, dy);
}

void Player::RightClickEvent()
{
	if (found && placeTimer.getTime() > 0.175f) {
		wManager.ModifyBlock(round(previousHitBlock.x), round(previousHitBlock.y), round(previousHitBlock.z), BlockType::Wooden_Plank);
		placeTimer.mark();
	}
	else {
		placeTimer.mark();
	}
}

void Player::LeftClickEvent()
{
	if (found && destroyTimer.getTime() > 0.175f) {
		wManager.ModifyBlock(hitBlockPos.x, hitBlockPos.y, hitBlockPos.z);
		destroyTimer.mark();
	}
	else {
		destroyTimer.mark();
	}
}

void Player::Draw()
{
	dt = moveTimer.mark();
	gfx.setCamera(cam.GetMatrix());
	auto model = DirectX::XMMatrixTranslation(hitBlockPos.x, hitBlockPos.y, hitBlockPos.z);

	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model * gfx.getCamera() * gfx.getProjection()),
		DirectX::XMMatrixTranspose(model)
	};

	blockSelector.UpdateConstantBuffer(tf);


	Renderer::Render(gfx, crosshair);

	if (hitBlockPos.y > -1) {
		gfx.RenderWireframe();
		Renderer::Render(gfx, blockSelector);
		gfx.RenderSolid();
	}
}
