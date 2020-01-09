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
GDIPlusManager gdipm;

Game::Game(size_t width, size_t height)
	: wnd(width, height), wManager(wnd.Gfx()), player(wnd.Gfx(), wManager), test(wnd.Gfx())
{

	const int area = 25;

	srand(time(0));

	worldScale = (rand() % (7 - 2 + 1)) + 7;
	waterScale = (worldScale - 1) * 5;

	noise.SetNoiseType(FastNoise::NoiseType::ValueFractal);
	noise.SetFractalOctaves(1);
	noise.SetFractalType(FastNoise::FractalType::Billow);
	noise.SetFrequency(0.01f);
	noise.SetSeed(time(0));
}

void Game::doFrame()
{
	while (!exit) {
		wnd.Gfx().BeginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);

		while (auto e = wnd.kbd.ReadKey())
		{

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

#include "MathFunctions.h"

void Game::MakeChunkThread()
{
	while (!exit) {
		if (meshEverything) {
			wManager.GenerateMeshes();
			meshEverything = false;
		}
		static Position oldpos = {0, 1, 0};
		Position pos = player.GetPositon();
		pos = Position(
			(pos.x - FixedMod(pos.x, BasicChunk::chunkSize)),
			(pos.y - FixedMod(pos.y, BasicChunk::chunkSize)),
			(pos.z - FixedMod(pos.z, BasicChunk::chunkSize))
		);
		auto orig = pos;
		pos.y = 0;
		std::optional<std::vector<BasicChunk*>> chunks;
		auto GetBlock = [&chunks](int x, int y, int z) {
			auto chunk = (*chunks)[y / 16];
			return &chunk->blocks[chunk->FlatIndex(x, y, z)];
		};
		const int area = 5 * BasicChunk::chunkSize;
		if (pos != oldpos) {
			for (int areaX = orig.x - area; areaX < orig.x + area; areaX += BasicChunk::chunkSize) {
				pos.x = areaX;
				for (int areaZ = orig.z - area; areaZ < orig.z + area; areaZ += BasicChunk::chunkSize) {
					pos.z = areaZ;
					if ((chunks = wManager.CreateChunkAtPlayerPos(pos)) != std::nullopt) {
						for (int x = 0; x < BasicChunk::chunkSize; x++) {
							for (int z = 0; z < BasicChunk::chunkSize; z++) {
								float height = (noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * BasicChunk::chunkSize;
								if (height < 2.0f) height = 2.0f;
								for (int y = 0; y < height * worldScale; y++) {
									auto block = GetBlock(x, y, z);
									if(y == 0) block->SetBlockType(Block::BlockType::Bedrock);
									else if (y >= waterScale) {
										if ((float)y > (height * worldScale) * 0.85f) {
											block->SetBlockType(Block::BlockType::Grass);
											//if (rand() % 1000 == 999) TreeGenerator::GenerateTree(player.evtManager, {pos.x + x, y + 1, pos.z + z});
										}
										else if ((float)y > (height * worldScale) * 0.65f && (float)y < (height * worldScale) * 0.85f)
											block->SetBlockType(Block::BlockType::Dirt);
										else
											block->SetBlockType(Block::BlockType::Stone);
									}
									else {
										if ((float)y > (height * worldScale) * 0.95f)
											block->SetBlockType(Block::BlockType::Dirt);
										else
											block->SetBlockType(Block::BlockType::Stone);
									}
								}
								for (int y = 1; y < waterScale; y++) {
									auto block = GetBlock(x, y, z);
									if (block->GetBlockType() == Block::BlockType::Air) {
										block->SetBlockType(Block::BlockType::Water);
									}
								}
							}
						}
						for (auto& chunk : *chunks) {
							wManager.GenerateMesh(*chunk);
						}
					}
				}
			}
			oldpos = orig;
		}
	}
}

int Game::start()
{
	std::thread tr(&Game::doFrame, this);
	std::thread chunktr(&Game::MakeChunkThread, this);
	while (1) {
		if (const auto result = Window::processMessages()) {
			exit = true;
			tr.join();
			chunktr.join();
			return result.value();
		}
	}
}

Game::~Game() {}
