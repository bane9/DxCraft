#include "Game.h"
#include "ThreadSettings.h"
#include <optional>
#include <memory>
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <math.h>
#include <algorithm>
#include "BillBoard.h"
#include "FastNoise.h"
#include "TreeGenerator.h"
#include <algorithm>
#include <ctime>
#include "EventManager.h"
#include "MathFunctions.h"

GDIPlusManager gdipm;

Game::Game(size_t width, size_t height)
	: wnd(width, height), wManager(wnd.Gfx()), player(wnd.Gfx(), wManager), test(wnd.Gfx())
{
	int area = 5;
	/*for (int x = -area; x < area * 2; x++) {
		for (int z = -area; z < area * 2; z++) {
			wManager.CreateChunk(x * 16, 0, z * 16);
		}
	}*/
	
}

void Game::DoFrame()
{
	while (!exit) {
		wnd.Gfx().BeginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);

		static Position oldpos = { 0, 1, 0 };
		Position pos = player.GetPositon();
		pos = Position(
			(pos.x - FixedMod(pos.x, Chunk::ChunkSize)),
			0,
			(pos.z - FixedMod(pos.z, Chunk::ChunkSize))
		);
		auto orig = pos;
		if (pos != oldpos) {
			wManager.UnloadChunks(pos, area);
			for (int areaX = orig.x - area; areaX < orig.x + area; areaX += Chunk::ChunkSize / 2) {
				pos.x = areaX;
				for (int areaZ = orig.z - area; areaZ < orig.z + area; areaZ += Chunk::ChunkSize / 2) {
					pos.z = areaZ;
					wManager.CreateChunkAtPlayerPos(pos);
				}
			}
			oldpos = orig;
		}
		wManager.wGen.Loop();

		while (auto e = wnd.kbd.ReadKey()) {

			if (e->GetCode() == 'W') {
				if (e->isPress()) {
					if (sprintTimer.GetTime() < 0.15f)
						player.SetSpeed(8.0f);
					else player.SetSpeed(4.0f);
				}
				else {
					player.SetSpeed(4.0f);
				}
				sprintTimer.Mark();
			}

			if (!e->isPress())
			{
				continue;
			}

			switch (e->GetCode())
			{
			case VK_ESCAPE:
				if (!showCursor)
				{
					wnd.enableCursor();
					showCursor = true;
					wnd.mouse.DisableRaw();
				}
				break;
			case 'N':
				player.ChangeBlock();
				break;
			case 'O':
				meshEverything = true;
				break;
			case VK_SPACE:
				if (jumpTimer.GetTime() < 0.2f) player.ToggleFlying();
				jumpTimer.Mark();
				break;
			case 'U':
			{
				if (!Evt::GlobalEvt.HasDataKey("farZ") || !Evt::GlobalEvt.HasDataKey("aspect ratio")) break;
				float farZ = Evt::GlobalEvt["farZ"];
				farZ -= 250.0f; 
				area -= Chunk::ChunkSize;
				float aspectRatio = Evt::GlobalEvt["aspect ratio"];
				Evt::GlobalEvt["farZ"] = farZ;
				Evt::GlobalEvt("Frustum Update", aspectRatio, farZ);
				break;
			}
			case 'I':
			{
				if (!Evt::GlobalEvt.HasDataKey("farZ") || !Evt::GlobalEvt.HasDataKey("aspect ratio")) break;
				float farZ = Evt::GlobalEvt["farZ"];
				farZ += 250.0f;
				area += Chunk::ChunkSize;
				float aspectRatio = Evt::GlobalEvt["aspect ratio"];
				Evt::GlobalEvt["farZ"] = farZ;
				Evt::GlobalEvt("Frustum Update", aspectRatio, farZ);
				break;
			}
			}
		}

		if (!showCursor)
		{
			if (wnd.kbd.KeyIsPressed('W'))
			{
				player.MoveForward();
			}
			if (wnd.kbd.KeyIsPressed('A'))
			{
				player.MoveLeft();
			}
			if (wnd.kbd.KeyIsPressed('S'))
			{
				player.MoveBackward();
			}
			if (wnd.kbd.KeyIsPressed('D'))
			{
				player.MoveRigth();
			}
			if (wnd.kbd.KeyIsPressed(VK_SPACE))
			{
				player.MoveUp();
			}
			if (wnd.kbd.KeyIsPressed(VK_SHIFT))
			{
				player.MoveDown();
			}
			
			while (auto wd = wnd.mouse.Read()) {
				if(wd->GetType() == Mouse::Event::Type::WheelUp) player.ChangeBlock();
				else if (wd->GetType() == Mouse::Event::Type::WheelDown) player.ChangeBlock(true);
			}
		}

		while (auto ms = wnd.mouse.Read()) {
			if (showCursor && ms->GetType() == Mouse::Event::Type::LPress && wnd.mouse.IsInWindow()) {
				wnd.disableCursor();
				showCursor = false;
				wnd.mouse.EnableRaw();
			}
		}

		while (const auto delta = wnd.mouse.ReadRawDelta())
		{
			if (!showCursor)
			{
				player.RotateCamera(delta->x, delta->y);
			}
		}

		auto asd = wnd.Gfx().GetFrametime();

		if (ImGui::Begin("Performance")) {
			ImGui::Text("Framerate: %.3f fps", 1000.0f / wnd.Gfx().GetFrametime());
			ImGui::Text("Frametime: %.3f ms",  wnd.Gfx().GetFrametime());
			ImGui::End();
		}

		if (wnd.mouse.LeftIsPressed()) {
			player.LeftClickEvent();
		}
		else if (wnd.mouse.RightIsPressed()) {
			player.RightClickEvent();
		}

		player.LoopThenDraw();


		auto model = DirectX::XMMatrixTranslation(0, 16, 0);
		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * wnd.Gfx().getCamera() * wnd.Gfx().getProjection()),
			DirectX::XMMatrixTranspose(model)
		};

		test.UpdateVScBuf(tf);
		
		wManager.RenderChunks(player.GetCamera());

		wnd.Gfx().EndFrame();
	}
}

void Game::MakeChunkThread()
{
	
	//while (!exit) {
	//	if (meshEverything) {
	//		wManager.GenerateMeshes();
	//		meshEverything = false;
	//	}
	//	static Position oldpos = {0, 1, 0};
	//	Position pos = player.GetPositon();
	//	pos = Position(
	//		(pos.x - FixedMod(pos.x, Chunk::ChunkSize)),
	//		0,
	//		(pos.z - FixedMod(pos.z, Chunk::ChunkSize))
	//	);
	//	auto orig = pos;
	//	std::optional<std::vector<std::shared_ptr<Chunk>>> chunks;
	//	auto GetBlock = [&chunks](int x, int y, int z) {
	//		return &(*(*chunks)[y / 16])(x, y, z);
	//	};
	//	if (pos != oldpos) {
	//		wManager.UnloadChunks(pos, area);
	//		for (int areaX = orig.x - area; areaX < orig.x + area; areaX += Chunk::ChunkSize / 2) {
	//			pos.x = areaX;
	//			for (int areaZ = orig.z - area; areaZ < orig.z + area; areaZ += Chunk::ChunkSize / 2) {
	//				pos.z = areaZ;
	//				if ((chunks = wManager.CreateChunkAtPlayerPos(pos)) != std::nullopt) {
	//					for (int x = 0; x < Chunk::ChunkSize; x++) {
	//						for (int z = 0; z < Chunk::ChunkSize; z++) {
	//							constexpr float prescale = 50.0f;
	//							float height = prescale + std::clamp(
	//								(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * (Chunk::ChunkSize - 1) * worldScale,
	//								0.0f,
	//								205.0f);
	//							for (int y = 0; y < height; y++) {
	//								auto block = GetBlock(x, y, z);
	//								const float scale = height;
	//								if(y == 0) block->SetBlockType(Block::BlockType::Bedrock);
	//								else if (y >= prescale + waterScale) {
	//									if (y > scale * 0.98f)
	//										block->SetBlockType(Block::BlockType::Grass);
	//									else if (y > scale * 0.75f && y < scale * 0.98f)
	//										block->SetBlockType(Block::BlockType::Dirt);
	//									else
	//										block->SetBlockType(Block::BlockType::Stone);
	//								}
	//								else {
	//									if (y > scale * 0.95f)
	//										block->SetBlockType(Block::BlockType::Dirt);
	//									else
	//										block->SetBlockType(Block::BlockType::Stone);
	//								}
	//							}
	//							for (int y = prescale; y < prescale + waterScale + 1; y++) {
	//								auto block = GetBlock(x, y, z);
	//								if (block->GetBlockType() == Block::BlockType::Air) {
	//									block->SetBlockType(Block::BlockType::Water);
	//								}
	//								else {
	//									static const auto InBounds = [](const int x, const int y, const int z) {
	//										return x >= 0 && y >= 0 && z >= 0 && x < 16 && y < 16 * 16 && z < 16;
	//									};
	//									if (InBounds(x + 1, y, z)) {
	//										auto blk = GetBlock(x + 1, y, z);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//									if (InBounds(x - 1, y, z)) {
	//										auto blk = GetBlock(x - 1, y, z);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//									if (InBounds(x, y + 1, z)) {
	//										auto blk = GetBlock(x, y + 1, z);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//									if (InBounds(x, y - 1, z)) {
	//										auto blk = GetBlock(x, y - 1, z);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//									if (InBounds(x, y, z + 1)) {
	//										auto blk = GetBlock(x, y, z + 1);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//									if (InBounds(x, y, z - 1)) {
	//										auto blk = GetBlock(x, y, z - 1);
	//										if (blk->GetBlockType() == Block::BlockType::Air || blk->GetBlockType() == Block::BlockType::Water) {
	//											block->SetBlockType(Block::BlockType::Wooden_Plank);
	//										}
	//									}
	//								}
	//							}
	//						}
	//					}
	//					Chunk* neigbourChunk = nullptr;
	//					for (auto& chunk : *chunks) {
	//						wManager.GenerateMesh(chunk);
	//						/*neigbourChunk = wManager.GetChunkFromBlock(chunk->x + 16, chunk->y, chunk->z);
	//						if (neigbourChunk != nullptr && (neigbourChunk->IndexBufferSize > 0 || neigbourChunk->AdditionalIndexBufferSize > 0)) wManager.GenerateMesh(*neigbourChunk);
	//						neigbourChunk = wManager.GetChunkFromBlock(chunk->x - 16, chunk->y, chunk->z);
	//						if (neigbourChunk != nullptr && (neigbourChunk->IndexBufferSize > 0 || neigbourChunk->AdditionalIndexBufferSize > 0)) wManager.GenerateMesh(*neigbourChunk);
	//						neigbourChunk = wManager.GetChunkFromBlock(chunk->x, chunk->y, chunk->z + 16);
	//						if (neigbourChunk != nullptr && (neigbourChunk->IndexBufferSize > 0 || neigbourChunk->AdditionalIndexBufferSize > 0)) wManager.GenerateMesh(*neigbourChunk);
	//						neigbourChunk = wManager.GetChunkFromBlock(chunk->x, chunk->y, chunk->z - 16);
	//						if (neigbourChunk != nullptr && (neigbourChunk->IndexBufferSize > 0 || neigbourChunk->AdditionalIndexBufferSize > 0)) wManager.GenerateMesh(*neigbourChunk);*/
	//					}
	//				}
	//			}
	//		}
	//		oldpos = orig;
	//	}
	//}
}

int Game::Start()
{
	std::thread tr(&Game::DoFrame, this);
	//std::thread chunktr(&Game::MakeChunkThread, this);
	while (1) {
		if (const auto result = Window::processMessages()) {
			exit = true;
			tr.join();
			//chunktr.join();
			return result.value();
		}
	}
}

Game::~Game() {}
