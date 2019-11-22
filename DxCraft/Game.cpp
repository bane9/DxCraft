#include "Game.h"
#include "ThreadSettings.h"
#include <optional>
#include <memory>
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <math.h>

GDIPlusManager gdipm;

Game::Game(size_t width, size_t height)
	: wnd(width, height), wManager(wnd.Gfx()), cameraRay(wManager)
{

	if (!showCursor) {
		wnd.disableCursor();
		wnd.mouse.EnableRaw();
	}

	cam.SetPos(0.0f, 25.0f, 0.0f);
	cam.setTravelSpeed(cameraSpeed);

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

	/*wManager.CreateChunk(0, 0, 0);
	wManager.CreateChunk(0, 1, 0, true);*/

	wManager.GenerateMeshes();
} 

void Game::doFrame()
{
	while (!exit) {
		const auto dt = timer.mark();
		wnd.Gfx().beginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);
		wnd.Gfx().setCamera(cam.GetMatrix());

		if (showCursor && ImGui::Begin("Settings")) {
			if (ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1.0f, 200.0f)) {
				cam.setTravelSpeed(cameraSpeed);
			}
			ImGui::End();
		}

		while (auto e = wnd.kbd.ReadKey())
		{
			if (e->GetCode() == VK_SHIFT) {
				if (e->isPress())
					cam.setTravelSpeed(cameraSpeed * 2);
				else cam.setTravelSpeed(cameraSpeed);
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
				cam.Translate({ 0.0f,0.0f,dt });
			}
			if (wnd.kbd.KeyIsPressed('A'))
			{
				cam.Translate({ -dt,0.0f,0.0f });
			}
			if (wnd.kbd.KeyIsPressed('S'))
			{
				cam.Translate({ 0.0f,0.0f,-dt });
			}
			if (wnd.kbd.KeyIsPressed('D'))
			{
				cam.Translate({ dt,0.0f,0.0f });
			}
			if (wnd.kbd.KeyIsPressed(VK_SPACE))
			{
				cam.Translate({ 0.0f,dt,0.0f });
			}
			if (wnd.kbd.KeyIsPressed(VK_CONTROL))
			{
				cam.Translate({ 0.0f,-dt,0.0f });
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
				cam.Rotate(delta->x, delta->y);
			}
		}

		if (ImGui::Begin("Performance")) {
			ImGui::Text("Framerate: %.3f fps", ImGui::GetIO().Framerate);
			ImGui::Text("Frametime: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::End();
		}

		wManager.Draw(wnd.Gfx());

		cameraRay.SetPositionAndDirection(cam.GetPos(), cam.GetPitch(), cam.GetYaw());
		auto old = cameraRay.GetVector();
		auto n = old;
		bool found = false;
		while (cameraRay.Next()) {
			auto block = wManager.GetBlock(n.x, n.y, n.z);
			if (block != nullptr && block->type != BlockType::Air) {
				found = true;
				break;
			}
			old = std::move(n);
			n = cameraRay.GetVector();
		}

		if (found) {
			if (wnd.mouse.LeftIsPressed() && destroyTimer.getTime() > 0.1f) {
				wManager.ModifyBlock(n.x, n.y, n.z);
				destroyTimer.mark();
			}
			else if (wnd.mouse.RightIsPressed() && placeTimer.getTime() > 0.175f) {
				wManager.ModifyBlock(old.x, old.y, old.z, BlockType::Wooden_Plank);
				placeTimer.mark();
			}
		}
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
