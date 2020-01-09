#include "Player.h"
#include "MathFunctions.h"
#include "imgui/imgui.h"
#include <algorithm>
#include "TreeGenerator.h"

#define FRAMETIME_COMPESATED(x) ((x) * (gfx.GetFrametime() / 16.667f))

Player::Player(Graphics& gfx, WorldManager& wManager)
	: 
	gfx(gfx),
	wManager(wManager),
	hitBlockPos(-1, -1, -1),
	blockSelector(gfx),
	crosshair(gfx),
	evtManager(wManager),
	hitDirection(0, 0, 0)
{
	crosshair.CreateVertexBuffer(Crosshair::NearSide.first);
	crosshair.CreateIndexBuffer(Crosshair::NearSide.second);
	crosshair.CreateVertexShader(L"CrosshairVS.cso", ied);
	crosshair.CreatePixelShader(L"CrossHairPS.cso");
	crosshair.UpdateVScBuf(DirectX::XMMatrixTranspose(gfx.getProjection()));

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
		auto t = cam.Translate({ 0.0f, 0.0f, FRAMETIME_COMPESATED(moveConstant) }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f, 0.0f, FRAMETIME_COMPESATED(moveConstant) }, speed * moveVelocity , flying);
	if (falling) {
		temp.x *= fallingDecreaseConstant;
		temp.z *= fallingDecreaseConstant;
	}
	else if(jumping) {
		temp.x *= jumpingDecreaseConstant;
		temp.z *= jumpingDecreaseConstant;
	}
	ResolveCollision(temp);
}

void Player::MoveBackward()
{
	if (flying) {
		auto t = cam.Translate({ 0.0f, 0.0f, FRAMETIME_COMPESATED(-moveConstant) }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ 0.0f , 0.0f, FRAMETIME_COMPESATED(-moveConstant) }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= fallingDecreaseConstant;
		temp.z *= fallingDecreaseConstant;
	}
	else if (jumping) {
		temp.x *= jumpingDecreaseConstant;
		temp.z *= jumpingDecreaseConstant;
	}
	ResolveCollision(temp);
}

void Player::MoveLeft()
{
	if (flying) {
		auto t = cam.Translate({ FRAMETIME_COMPESATED(-moveConstant), 0.0f, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ FRAMETIME_COMPESATED(-moveConstant), 0.0f, 0.0f }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= fallingDecreaseConstant;
		temp.z *= fallingDecreaseConstant;
	}
	else if (jumping) {
		temp.x *= jumpingDecreaseConstant;
		temp.z *= jumpingDecreaseConstant;
	}
	ResolveCollision(temp);
}

void Player::MoveRigth()
{
	if (flying) {
		auto t = cam.Translate({ FRAMETIME_COMPESATED(moveConstant), 0.0f, 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		return;
	}
	moveVelocity += velocityIncreaseConstant;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);
	auto temp = cam.Translate({ FRAMETIME_COMPESATED(moveConstant), 0.0f, 0.0f }, speed * moveVelocity, flying);
	if (falling) {
		temp.x *= fallingDecreaseConstant;
		temp.z *= fallingDecreaseConstant;
	}
	else if (jumping) {
		temp.x *= jumpingDecreaseConstant;
		temp.z *= jumpingDecreaseConstant;
	}
	ResolveCollision(temp);
}

void Player::MoveUp(bool external)
{
	auto asd = jumpTimer.GetTime();
	if ((jumpTimer.GetTime() > 0.175f && !falling) || jumping || flying) {
		if (!flying) jumping = true;
		if (!external && flying) {
			auto t = cam.Translate({ 0.0f, FRAMETIME_COMPESATED(jumpFallConstant), 0.0f }, speed * flyingSpeedModifier, flying);
			auto camPos = cam.GetPos();
			cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
		}
		else if (external) {
			auto temp = cam.Translate({ 0.0f, FRAMETIME_COMPESATED(jumpFallConstant), 0.0f }, jumpDistance - fallVelocity);
			temp.x += momentum.x;
			temp.z += momentum.z;
			ResolveCollision(temp);
		}
		jumpTimer.Mark();
	}
}

void Player::MoveDown(bool external)
{
	if (!flying && !external) return;
	else if (!external) {
		auto t = cam.Translate({ 0.0f, FRAMETIME_COMPESATED(-jumpFallConstant), 0.0f }, speed * flyingSpeedModifier, flying);
		auto camPos = cam.GetPos();
		cam.SetPos(camPos.x + t.x, camPos.y + t.y, camPos.z + t.z);
	}
	else {
		auto temp = cam.Translate({ 0.0f, FRAMETIME_COMPESATED(-jumpFallConstant), 0.0f }, fallVelocity);
		temp.x += momentum.x;
		temp.z += momentum.z;
		ResolveCollision(temp);
	}
}

void Player::CastRay()
{
	//return;
	cameraRay.SetPositionAndDirection(cam.GetPos(), cam.GetPitch(), cam.GetYaw());
	previousHitBlock = cameraRay.GetVector();
	hitBlock = previousHitBlock;
	found = false;

	hitBlockPos = { -1, -1, -1 };

	while (cameraRay.Next()) {
		auto block = wManager.GetBlock(hitBlock);
		if (block != nullptr && block->GetBlockType() != Block::BlockType::Air && !block->IsLiquid()) {
			auto pos = block->GetPosition();
			AABB aabb = block->GetAABB();
			aabb.SetPosition({static_cast<float>(pos.x), static_cast<float>(pos.y) - 0.5f, static_cast<float>(pos.z)});
			if (aabb.IsPointInside(hitBlock)) {
				hitBlockPos = { pos.x, pos.y, pos.z };
				found = true;
				blockSelector.SetType(block->GetSelectorType());
				hitDirection = { 0, (int)(round(hitBlock.y) - round(previousHitBlock.y)), 0 };
				hitDirection.x = hitDirection.y != 0 ? 0 : (int)(round(hitBlock.x) - round(previousHitBlock.x));
				hitDirection.z = hitDirection.y != 0 || hitDirection.z != 0 ? 0 : (int)(round(hitBlock.z) - round(previousHitBlock.z));
				break;
			}
		}
		previousHitBlock = std::move(hitBlock);
		hitBlock = cameraRay.GetVector();
	}
}

void Player::PlaceLiquid()
{
	BlockEventManager::Event evt = {};
	evt.blockPosition = previousHitBlock;
	evt.blockType = Block::BlockType::Water;
	evt.eventType = BlockEventManager::Event::EventType::PLACE_BLOCK;
	evt.water_level = 7;
	evtManager.AddEvent(evt);
}

void Player::RotateCamera(float dx, float dy)
{
	cam.Rotate(dx, dy);
}

void Player::RightClickEvent()
{
	if (found && placeTimer.GetTime() > 0.075f) {
		auto camPos = cam.GetPos();
		auto camPosUpper = Position(camPos);
		auto camPosLower = Position(DirectX::XMFLOAT3(camPos.x, camPos.y - 1.0f, camPos.z));
		auto block = wManager.GetBlock(previousHitBlock);
		if (block != nullptr) {
			auto p = block->GetPosition();
			if (!(p == camPosUpper || p == camPosLower)) {
				auto checkSapling = wManager.GetBlock(hitBlock);
				if (checkSapling != nullptr && checkSapling->GetBlockType() == Block::BlockType::Oak_Sapling) {
					TreeGenerator::GenerateTree(evtManager, hitBlockPos);
				}
				else if (type == Block::BlockType::Water) {
					PlaceLiquid();
				}
				else {
					evtManager.PlaceBlock(previousHitBlock, hitDirection, type);
				}
			}
		}

	}
	placeTimer.Mark();
}

void Player::LeftClickEvent()
{
	if (found && destroyTimer.GetTime() > 0.075f)
		evtManager.RemoveBlock(hitBlockPos);

	destroyTimer.Mark();
}

void Player::LoopThenDraw()
{
	evtManager.Loop();
	if (jumping && falling) {
		jumping = false;
		falling = false;
	}

	if (ImGui::Begin("Selected block")) {
		ImGui::Text("%s", blockName.c_str());
		ImGui::End();
	}

	CastRay();

	if (false && ImGui::Begin("Hit direction")) {
		if(!found) ImGui::Text("%s", "Not hit");
		else ImGui::Text("%i %i %i", hitDirection.x, hitDirection.y, hitDirection.z);
		ImGui::End();
	}

	if (!flying && !jumping) {
		fallVelocity += FRAMETIME_COMPESATED(0.125f * fallSpeedModifier);
		fallVelocity = std::clamp(fallVelocity, fallMinBound, 100.0f);
		auto cPos = cam.GetPos();
		auto block = wManager.GetBlock(DirectX::XMFLOAT3{cPos.x, cPos.y - 1.0f, cPos.z});
		if (block == nullptr || !block->IsCollideable()) {
			bool fall = true;
			if(fall) MoveDown(true);
		}
		if (cam.GetPos().y < -15.0f) cam.SetPos(0.0f, 25.0f, 0.0f);
	}
	if (jumping && !falling) {
		jumpVelocity -= FRAMETIME_COMPESATED(0.125f * jumpSpeedModifier);

		if (jumpVelocity < -jumpDistance) {
			jumping = false;
			fallVelocity = 0.25f;
			jumpVelocity = 0.0f;
			falling = true;
		}
		else {
			MoveUp(true);
		}
	}

	if (!falling && !jumping) {
		fallVelocity = fallMinBound;

		if (fabs(modf(round(momentum.x), &momentum.x)) < 0.000175f)
			momentum.x -= 0.00175f * sgn(momentum.x);
		else momentum.x = 0.0f;

		if (fabs(modf(round(momentum.z), &momentum.z)) < 0.000175f)
			momentum.z -= 0.00175f * sgn(momentum.z);
		else momentum.z = 0.0f;
	}

	moveVelocity -= velocityIncreaseConstant * 0.2f;
	moveVelocity = std::clamp(moveVelocity, velocityMinBound, velocityMaxBound);

	gfx.setCamera(cam.GetMatrix());

	blockSelector.SetPosition(hitBlockPos, gfx.getCamera(), gfx.getProjection());

	crosshair.Render();

	if (hitBlockPos.y > -1) {
		blockSelector.Render(gfx);
	}
	velocityMaxBound = 1.0f;
	
}

void Player::ChangeBlock(bool decrement)
{
	if (!decrement)
		++blockIndex;
	else
		--blockIndex;
	if (blockIndex > 17) blockIndex = 1;
	else if (blockIndex < 1) blockIndex = 17;
	
	switch (blockIndex) {
		case 1:
			type = Block::BlockType::Stone;
			blockName = "Stone";
			break;
		case 2:
			type = Block::BlockType::Grass;
			blockName = "Grass";
			break;
		case 3:
			type = Block::BlockType::Dirt;
			blockName = "Dirt";
			break;
		case 4:
			type = Block::BlockType::Wooden_Plank;
			blockName = "Wooden Plank";
			break;
		case 5:
			type = Block::BlockType::Glass;
			blockName = "Glass";
			break;
		case 6: 
			type = Block::BlockType::Sugar_Cane;
			blockName = "Sugar Cane";
			break;
		case 7:
			type = Block::BlockType::Dandelion;
			blockName = "Dandelion";
			break;
		case 8:
			type = Block::BlockType::Poppy;
			blockName = "Poppy";
			break;
		case 9:
			type = Block::BlockType::Brown_Mushroom;
			blockName = "Brown Mushroom";
			break;
		case 10:
			type = Block::BlockType::Red_Mushroom;
			blockName = "Red Mushroom";
			break;
		case 11:
			type = Block::BlockType::Oak_Sapling;
			blockName = "Oak Sapling";
			break;
		case 12:
			type = Block::BlockType::Birch_Sapling;
			blockName = "Birch Sapling";
			break;
		case 13:
			type = Block::BlockType::Dark_Oak_Sapling;
			blockName = "Dark Oak Sapling";
			break;
		case 14:
			type = Block::BlockType::Oak_Wood;
			blockName = "Oak Wood";
			break;
		case 15:
			type = Block::BlockType::Leaves;
			blockName = "Leaves";
			break;
		case 16:
			type = Block::BlockType::Water;
			blockName = "Water";
			break;
		case 17:
			type = Block::BlockType::Lava;
			blockName = "Lava";
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
	}
}

Position Player::GetPositon() noexcept
{
	return { cam.GetPos() };
}

#define VALID_BLOCK(x) x != nullptr && x->IsCollideable()

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

	const float offsetX =		 blockFace * sgn(delta.x);
	const float offsetY =		 1.5f      * sgn(delta.y);
	const float offsetYLower =  -1.0f      * sgn(pos.y);
	const float offsetZ =		 blockFace * sgn(delta.z);

	Block* block = nullptr;

	if (delta.y < 0.0f) {
		bool check = true;
		block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x, pos.y + delta.y + offsetY, pos.z });
		if (VALID_BLOCK(block)) {
			if (modf(round(pos.y), &pos.y) < 0.001f) {
				delta.y = 0;
				falling = false;
				check = false;
			}
		}
		else if (check && !jumping) falling = true;
	}
	else if (delta.y > 0.0f) {
		block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x, pos.y - (offsetYLower * (Faces::side * 0.5f)), pos.z });
		if (VALID_BLOCK(block)) {
			delta.y = 0;
			jumping = false;
			jumpVelocity = 0.0f;
			moveVelocity = velocityMinBound;
			velocityMaxBound = collisionMaxBoud;
			falling = true;
		}
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y, pos.z });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetYLower, pos.z });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x, pos.y, pos.z + delta.z + offsetZ });
	if (VALID_BLOCK(block)){
		delta.z = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x, pos.y + offsetYLower, pos.z + delta.z + offsetZ });
	if (VALID_BLOCK(block)){
		delta.z = 0;
		velocityMaxBound = collisionMaxBoud;
	}


	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetY, pos.z + sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetY, pos.z - sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetY, pos.z + sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetY, pos.z - sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}


	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetYLower, pos.z + sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetYLower, pos.z - sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetYLower, pos.z + sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}

	block = wManager.GetBlock(DirectX::XMFLOAT3{ pos.x + delta.x + offsetX, pos.y + offsetYLower, pos.z - sgn(pos.z) * diagonalBlockFace });
	if (VALID_BLOCK(block)) {
		delta.x = 0;
		velocityMaxBound = collisionMaxBoud;
	}
	

	cam.SetPos(pos.x + delta.x, pos.y + delta.y, pos.z + delta.z);
	
	if (!jumping && !falling) {
		momentum.x = delta.x;
		momentum.z = delta.z;
	}
}
