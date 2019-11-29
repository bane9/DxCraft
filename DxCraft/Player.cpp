#include "Player.h"
#include "MathFunctions.h"
#include "imgui/imgui.h"
#include <algorithm>
#include "RenderDataFactory.h"

Player::Player(Graphics& gfx, WorldManager& wManager)
	: 
	gfx(gfx),
	wManager(wManager),
	hitBlockPos(-1, -1, -1)
{
	RenderDataFactory::CreateVertexBuffer(gfx, crosshair, Crosshair::NearSide.first);
	RenderDataFactory::CreateIndexBuffer(gfx, crosshair, Crosshair::NearSide.second);
	RenderDataFactory::CreateVertexShader(gfx, crosshair, L"CrosshairVS.cso", ied);
	RenderDataFactory::CreatePixelShader(gfx, crosshair, L"CrossHairPS.cso");
	RenderDataFactory::UpdateVScBuf(gfx, crosshair, DirectX::XMMatrixTranspose(gfx.getProjection()));

	RenderDataFactory::CreateVertexBuffer(gfx, blockSelector, BlockSelector::Cube.first);
	RenderDataFactory::CreateIndexBuffer(gfx, blockSelector, BlockSelector::Cube.second);
	RenderDataFactory::CreateVertexShader(gfx, blockSelector, L"SelectionVS.cso", ied);
	RenderDataFactory::CreatePixelShader(gfx, blockSelector, L"SelectionPS.cso");

	cam.SetPos(0.0f, 25.0f, 0.0f);
}

void Player::SetSpeed(float speed)
{
	this->speed = speed;
}

float Player::GetVelocity()
{
	return speed;
}

void Player::MoveForward()
{
	if (flying) {
		auto t = cam.Translate({ 0.0f, 0.0f, dt }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f, 0.0f, dt }, speed * (flying ? flyingSpeedModifier : moveVelocity), flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveBackward()
{
	if (flying) {
		auto t = cam.Translate({ 0.0f, 0.0f, -dt }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f ,0.0f, -dt }, speed * (flying ? flyingSpeedModifier : moveVelocity), flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveLeft()
{
	if (flying) {
		auto t = cam.Translate({ -dt, 0.0f, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ -dt, 0.0f, 0.0f }, speed * (flying ? flyingSpeedModifier : moveVelocity), flying);
	if (falling) {
		temp.x *= 0.35f;
		temp.y *= 0.35f;
		temp.z *= 0.35f;
	}
	ResolveCollision(temp);
}

void Player::MoveRigth()
{
	if (flying) {
		auto t = cam.Translate({ dt, 0.0f, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ dt, 0.0f, 0.0f }, speed * (flying ? flyingSpeedModifier : moveVelocity), flying);
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
	if (!external && flying) {
		auto t = cam.Translate({ 0.0f, dt, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
	}
	else if (external) {
		auto temp = cam.Translate({ 0.0f, dt, 0.0f }, jumpDistance - fallVelocity);
		temp.x += momentum.x * 0.1f;
		temp.z += momentum.z * 0.1f;
		ResolveCollision(temp);
	}
}

void Player::MoveDown(bool external)
{
	if (!flying && !external) return;
	else if (!external) {
		auto t = cam.Translate({ 0.0f, -dt, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
	}
	else {
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

	RenderDataFactory::UpdateVScBuf(gfx, blockSelector, tf);

	Renderer::DrawIndexed(gfx, crosshair);

	if (hitBlockPos.y > -1) {
		gfx.RenderWireframe();
		Renderer::DrawIndexed(gfx, blockSelector);
		gfx.RenderSolid();
	}
}

void Player::ChangeBlock(bool decrement)
{
	if (!decrement)
		++blockIndex;
	else
		--blockIndex;
	if (blockIndex > 5) blockIndex = 1;
	else if (blockIndex < 1) blockIndex = 5;
	
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

Camera& Player::GetCamera() noexcept
{
	return cam;
}

void Player::ToggleFlying() noexcept
{
	flying = !flying;
	collision = !collision;
	if (!flying) {
		fallVelocity = 0.25f;
		jumpVelocity = 0.0f;
		fallTimer.mark();
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
	const float offsetY =		 1.5f   * sgn(delta.y);
	const float offsetYLower =  -1.0f   * sgn(pos.y);
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
