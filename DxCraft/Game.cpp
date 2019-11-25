#include "Game.h"
#include "ThreadSettings.h"
#include <optional>
#include <memory>
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <math.h>
#include <algorithm>
#include "Renderer.h"
#include "BlockSelector.h"

GDIPlusManager gdipm;

Game::Game(size_t width, size_t height)
	: wnd(width, height), wManager(wnd.Gfx()), player(wnd.Gfx(), wManager)
{

	const int area = 4;
	for (int x = -area / 2; x < area / 2; x++) {
		for (int z = -area / 2; z < area / 2; z++) {
			wManager.CreateChunk(x, 0, z);
		}
	}

	for (int x = -area / 2; x < area / 2; x++) {
		for (int z = -area / 2; z < area / 2; z++) {
			wManager.CreateChunk(x, 1, z, true);
		}
	}

	//wManager.CreateChunk(0, 0, 0);
	//wManager.CreateChunk(0, 1, 0, true);
	wManager.GenerateMeshes();

} 

void Game::doFrame()
{
	while (!exit) {
		wnd.Gfx().beginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);

		while (auto e = wnd.kbd.ReadKey())
		{

			if (e->GetCode() == VK_SHIFT) {
				if (e->isPress())
					player.SetVelocity(20.0f);
				else player.SetVelocity(10.0f);
				continue;
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
			if (wnd.kbd.KeyIsPressed(VK_CONTROL))
			{
				player.MoveDown();
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

		if (ImGui::Begin("Performance")) {
			ImGui::Text("Framerate: %.3f fps", ImGui::GetIO().Framerate);
			ImGui::Text("Frametime: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::End();
		}



		if (wnd.mouse.LeftIsPressed()) {
			player.LeftClickEvent();
		}
		else if (wnd.mouse.RightIsPressed()) {
			player.RightClickEvent();
		}

		player.CastRay();

		wManager.Draw(wnd.Gfx());
	
		player.Draw();

		wnd.Gfx().endFrame();
	}
}

int Game::start()
{
	std::thread tr(&Game::doFrame, this);
	while (1) {
		if (const auto result = Window::processMessages()) {
			exit = true;
			tr.join();
			return result.value();
		}
	}
}

Game::~Game() {}
