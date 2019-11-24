#include "Player.h"
#include "MathFunctions.h"
#include "imgui/imgui.h"
#include <algorithm>

Player::Player(Graphics& gfx, WorldManager& wManager)
	: 
	gfx(gfx),
	wManager(wManager),
	crosshair(gfx, L"CrosshairVS.cso", L"CrossHairPS.cso", ied),
	blockSelector(gfx, L"SelectionVS.cso", L"SelectionPS.cso", ied),
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
	if (!falling || jumping) {
		moveVelocity += velocityIncreaseConstant;
		moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
		ResolveCollision(cam.Translate({ 0.0f, 0.0f, dt }, velocity * moveVelocity, flying));
	}
}

void Player::MoveBackward()
{
	if (!falling || jumping) {
		moveVelocity += velocityIncreaseConstant;
		moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
		ResolveCollision(cam.Translate({ 0.0f ,0.0f, -dt }, velocity * moveVelocity, flying));
	}
}

void Player::MoveLeft()
{
	if (!falling || jumping) {
		moveVelocity += velocityIncreaseConstant;
		moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
		ResolveCollision(cam.Translate({ -dt, 0.0f, 0.0f }, velocity * moveVelocity, flying));
	}
}

void Player::MoveRigth()
{
	if (!falling || jumping) {
		moveVelocity += velocityIncreaseConstant;
		moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
		ResolveCollision(cam.Translate({ dt, 0.0f, 0.0f }, velocity * moveVelocity, flying));
	}
}

void Player::MoveUp(bool external)
{
	if (!flying || !falling) jumping = true;
	if (external) {
		auto temp = cam.Translate({ 0.0f, dt, 0.0f }, jumpDistance - fallVelocity);
		temp.x += momentum.x * (falling ? 1.0f : 0.0f) * moveVelocity;
		temp.z += momentum.z * (falling ? 1.0f : 0.0f) * moveVelocity;
		ResolveCollision(temp);
	}
}

void Player::MoveDown(bool external)
{
	if (!flying && !external) return;
	if (external) {
		auto temp = cam.Translate({ 0.0f, -dt, 0.0f }, fallVelocity);
		temp.x += momentum.x * (falling ? 1.0f : 0.0f) * moveVelocity;
		temp.z += momentum.z * (falling ? 1.0f : 0.0f) * moveVelocity;
		ResolveCollision(temp);
	}
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
			hitBlockPos = { block->x, block->y, block->z };
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
	if (found && placeTimer.getTime() > 0.15f) {
		auto camPos = cam.GetPos();
		auto camPosLower = camPos;
		camPos.y -= 1 * sgn(camPos.y);
		if(abs(VectorDistance(previousHitBlock, camPos) < 0.75f) ||
			abs(VectorDistance(previousHitBlock, camPosLower) < 0.75f)) return;
		wManager.ModifyBlock(round(previousHitBlock.x), round(previousHitBlock.y), round(previousHitBlock.z), BlockType::Wooden_Plank);
		placeTimer.mark();
	}
	else {
		placeTimer.mark();
	}
}

void Player::LeftClickEvent()
{
	if (found && destroyTimer.getTime() > 0.15f) {
		wManager.ModifyBlock(hitBlockPos.x, hitBlockPos.y, hitBlockPos.z);
		destroyTimer.mark();
	}
	else {
		destroyTimer.mark();
	}
}

void Player::Draw()
{
	if (ImGui::Begin("Collision")) {
		if (ImGui::Checkbox("Noclip", &flying)) {
			collision = !flying;
		}
		ImGui::End();
	}

	if (!flying && !jumping) {
		fallVelocity += fallTimer.getTime() * 0.5f;
		fallVelocity = std::clamp(fallVelocity, 0.0f, 75.0f);
		MoveDown(true);
		if (cam.GetPos().y < -15) cam.SetPos(0.0f, 25.0f, 0.0f);
	}
	else if (jumping) {
		jumpVelocity -= fallTimer.getTime();

		if (jumpVelocity < -jumpDistance) {
			jumping = false;
			jumpVelocity = 0.0f;
		}
		else {
			falling = false;
			MoveUp(true);
		}
	}

	if (!falling && !jumping) {
		fallTimer.mark();
		fallVelocity = 0.0f;
	}
	else {
		moveVelocity += velocityIncreaseConstant * 0.2f;
		velocity = baseVelocity;
	}

	moveVelocity -= velocityIncreaseConstant * 0.2f;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);

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

void Player::ResolveCollision(DirectX::XMFLOAT3 delta)
{
	DirectX::XMFLOAT3 pos = cam.GetPos();

	if (!collision) {
		cam.SetPos(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		return;
	}

	if(delta.x == 0 && delta.z == 0) moveVelocity = 0.0f;

	const float offsetX =		0.5f   * sgn(delta.x);
	const float offsetY =		1.5f   * sgn(delta.y);
	const float offsetYLower =  -1.25f * sgn(pos.y);
	const float offsetZ =		0.5f   * sgn(delta.z);

	bool check = true;
	auto block = wManager.GetBlock(round(pos.x), round(pos.y + delta.y + offsetY), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air) {
		delta.y = 0;
		if (modf(round(pos.y), &pos.y) < 0.1f) {
			falling = false;
			check = false;
		}
		else {
			moveVelocity = 0.0f;
		}
	}
	else if (check) falling = true;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX), round(pos.y), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX), round(pos.y + offsetYLower), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x), round(pos.y), round(pos.z + delta.z + offsetZ));
	if (block != nullptr && block->type != BlockType::Air) delta.z = 0;

	block = wManager.GetBlock(round(pos.x), round(pos.y + offsetYLower), round(pos.z + delta.z + offsetZ));
	if (block != nullptr && block->type != BlockType::Air) delta.z = 0;


	const float offset = sgn(pos.x) * 0.125f;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y), round(pos.z + sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y), round(pos.z - sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y), round(pos.z + sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y), round(pos.z - sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;


	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetYLower), round(pos.z + sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetYLower), round(pos.z - sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetYLower), round(pos.z + sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetYLower), round(pos.z - sgn(pos.z) * 0.25));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;
	

	cam.SetPos(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
	

	if (delta.x) momentum.x = delta.x;
	if (delta.z) momentum.z = delta.z;
}
