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
	: wnd(width, height), wManager(wnd.Gfx()), player(wnd.Gfx(), wManager)
{
}

void Game::DoFrame()
{
	Position oldpos = { 0, 1, 0 };
	while (!exit) {
		wnd.Gfx().BeginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);

		Position pos = player.GetPositon();
		pos = Position(
			(pos.x - FixedMod(pos.x, Chunk::ChunkSize)),
			0,
			(pos.z - FixedMod(pos.z, Chunk::ChunkSize))
		);
		if (pos != oldpos) {
			positionQueue.push(pos);
			oldpos = pos;
		}

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
		
		wManager.RenderChunks(player.GetCamera());

		wnd.Gfx().EndFrame();
	}
}

void Game::MakeChunkThread()
{
	using namespace std::chrono_literals;
	while (!exit) {
		while (!exit && (positionQueue.empty() || wManager.lockThread)) std::this_thread::sleep_for(10ms);
		if (exit) return;
		Position pos = positionQueue.pop();
		wManager.creatingChunks = true;
		wManager.UnloadChunks(pos, area);
		auto orig = pos;
		for (int areaX = orig.x - area; areaX < orig.x + area; areaX += Chunk::ChunkSize) {
			pos.x = areaX;
			for (int areaZ = orig.z - area; areaZ < orig.z + area; areaZ += Chunk::ChunkSize) {
				pos.z = areaZ;
				wManager.CreateChunkAtPlayerPos(pos);
			}
		}
		wManager.creatingChunks = false;
	}
}

int Game::Start()
{
	std::thread tr(&Game::DoFrame, this);
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
