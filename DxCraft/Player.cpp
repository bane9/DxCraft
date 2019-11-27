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

void Player::SetVelocity(float speed)
{
	this->speed = speed;
}

float Player::GetVelocity()
{
	return speed;
}

void Player::MoveForward()
{
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f, 0.0f, dt }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveBackward()
{
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f ,0.0f, -dt }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveLeft()
{
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ -dt, 0.0f, 0.0f }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveRigth()
{
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ dt, 0.0f, 0.0f }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveUp(bool external)
{
	if (!flying) jumping = true;
	if (external) {
		auto temp = cam.Translate({ 0.0f, dt, 0.0f }, jumpDistance - fallVelocity);
		temp.x += momentum.x * 0.1f;
		temp.z += momentum.z * 0.1f;
		ResolveCollision(temp);
	}
}

void Player::MoveDown(bool external)
{
	if (!flying && !external) return;
	if (external) {
		auto temp = cam.Translate({ 0.0f, -dt, 0.0f }, fallVelocity);
		temp.x += momentum.x * (falling ? 1.0f : 0.0f) * 0.1f;
		temp.z += momentum.z * (falling ? 1.0f : 0.0f) * 0.1f;
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
	if (found && placeTimer.getTime() > 0.075f) {
		auto camPos = cam.GetPos();
		auto camPosLower = camPos;
		camPosLower.y -= 1.15f * sgn(camPosLower.y);
		if(fabs(VectorDistance(previousHitBlock, camPos) < 0.578f) ||
			fabs(VectorDistance(previousHitBlock, camPosLower) < 0.578f)) return;
		wManager.ModifyBlock(round(previousHitBlock.x), round(previousHitBlock.y), round(previousHitBlock.z), type);
		placeTimer.mark();
	}
	else {
		placeTimer.mark();
	}
}

void Player::LeftClickEvent()
{
	if (found && destroyTimer.getTime() > 0.075f) {
		wManager.ModifyBlock(hitBlockPos.x, hitBlockPos.y, hitBlockPos.z);
		destroyTimer.mark();
	}
	else {
		destroyTimer.mark();
	}
}

void Player::LoopThenDraw()
{
	if (ImGui::Begin("Collision")) {
		if (ImGui::Checkbox("Noclip", &flying)) {
			collision = !flying;
		}
		ImGui::End();
	}

	if (ImGui::Begin("Selected block")) {
		ImGui::Text("%s", blockName.c_str());
		ImGui::End();
	}

	CastRay();

	if (!flying) {
		fallVelocity += fallTimer.getTime();
		fallVelocity = std::clamp(fallVelocity, 0.25f, 20.0f);
		MoveDown(true);
		if (cam.GetPos().y < -15.0f) cam.SetPos(0.0f, 25.0f, 0.0f);
	}
	if (jumping) {
		jumpVelocity -= fallTimer.getTime();

		if (jumpVelocity < -jumpDistance) {
			jumping = false;
			jumpVelocity = 0.0f;
			fallTimer.mark();
		}
		else {
			MoveUp(true);
		}
	}

	if (!falling && !jumping) {
		fallTimer.mark();
		fallVelocity = 0.25f;

		if (fabs(modf(round(momentum.x), &momentum.x)) < 0.000175f)
			momentum.x -= 0.00175f * sgn(momentum.x);
		else momentum.x = 0.0f;

		if (fabs(modf(round(momentum.z), &momentum.z)) < 0.000175f)
			momentum.z -= 0.00175f * sgn(momentum.z);
		else momentum.z = 0.0f;
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

void Player::ChangeBlock()
{
	++blockIndex;
	if (blockIndex > 5) blockIndex = 1;
	switch (blockIndex) {
		case 1:
			type = BlockType::Stone;
			blockName = "Stone";
			break;
		case 2:
			type = BlockType::Grass;
			blockName = "Grass";
			break;
		case 3:
			type = BlockType::Dirt;
			blockName = "Dirt";
			break;
		case 4:
			type = BlockType::Wooden_Plank;
			blockName = "Wooden Plank";
			break;
		case 5:
			type = BlockType::Glass;
			blockName = "Glass";
			break;
	}
}

void Player::ResolveCollision(DirectX::XMFLOAT3 delta)
{
	DirectX::XMFLOAT3 pos = cam.GetPos();

	if (!collision) {
		cam.SetPos(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
		return;
	}

	if (delta.x == 0 && delta.z == 0) {
		moveVelocity = 0.0f;
	}

	const float offsetX =		 0.251f * sgn(delta.x);
	const float offsetY =		 1.5f * sgn(delta.y);
	const float offsetYLower =  -1.0f * sgn(pos.y);
	const float offsetZ =		 0.251f * sgn(delta.z);

	bool check = true;
	auto block = wManager.GetBlock(round(pos.x), round(pos.y + delta.y + offsetY), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air && delta.y < 0.0f) {
		delta.y = 0;
		if (modf(round(pos.y), &pos.y) < 0.00001f) {
			falling = false;
			check = false;
		}
	}
	else if (check) falling = true;

	if (delta.y > 0.0f) {
		block = wManager.GetBlock(round(pos.x), round(pos.y - (offsetYLower * 0.25f)), round(pos.z));
		if (block != nullptr && block->type != BlockType::Air) {
			delta.y = 0;
			jumping = false;
			jumpVelocity = 0.0f;
			fallTimer.mark();
		}
	}

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX), round(pos.y), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX), round(pos.y + offsetYLower), round(pos.z));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x), round(pos.y), round(pos.z + delta.z + offsetZ));
	if (block != nullptr && block->type != BlockType::Air) delta.z = 0;

	block = wManager.GetBlock(round(pos.x), round(pos.y + offsetYLower), round(pos.z + delta.z + offsetZ));
	if (block != nullptr && block->type != BlockType::Air) delta.z = 0;


	const float offset = sgn(pos.x) * 0.0f;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetY), round(pos.z + sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetY), round(pos.z - sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetY), round(pos.z + sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetY), round(pos.z - sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;


	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetYLower), round(pos.z + sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX + offset), round(pos.y + offsetYLower), round(pos.z - sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetYLower), round(pos.z + sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;

	block = wManager.GetBlock(round(pos.x + delta.x + offsetX - offset), round(pos.y + offsetYLower), round(pos.z - sgn(pos.z) * 0.25f));
	if (block != nullptr && block->type != BlockType::Air) delta.x = 0;
	

	cam.SetPos(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
	
	if (!jumping) {
		if (delta.x) momentum.x = delta.x;
		if (delta.z) momentum.z = delta.z;
	}
}
