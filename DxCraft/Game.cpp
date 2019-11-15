#include "Game.h"
#include "ThreadSettings.h"
#include <optional>
#include <memory>
#include "GDIPlusManager.h"
#include "imgui/imgui.h"


GDIPlusManager gdipm;

Game::Game(size_t width, size_t height)
	: wnd(width, height), wManager(wnd.Gfx())
{

	if (!showCursor) {
		wnd.disableCursor();
		wnd.mouse.EnableRaw();
	}

	cam.SetPos(-3.0f, 3.0f, -25.0f);
	cam.setTravelSpeed(cameraSpeed);

	wManager.CreateChunk(0, 0, 0);
	wManager.CreateChunk(16, 0, 0);
} 

void Game::doFrame()
{
#ifdef THREADED
	while (!exit) {
#endif
		const auto dt = timer.mark();
		wnd.Gfx().beginFrame(0.5f * skyIntesity, 0.91f * skyIntesity, 1.0f * skyIntesity);
		wnd.Gfx().setCamera(cam.GetMatrix());

		while (auto e = wnd.kbd.ReadKey())
		{
			if (e->GetCode() == VK_SHIFT) {
				if (e->isPress())
					cam.setTravelSpeed(30.0f);
				else cam.setTravelSpeed(15.0f);
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

		while (auto e = wnd.kbd.ReadKey())
		{
			if (e->GetCode() == VK_SHIFT) {
				if (e->isPress())
					cam.setTravelSpeed(40.0f);
				else cam.setTravelSpeed(20.0f);
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

		wManager.Draw();

		wnd.Gfx().endFrame();
#ifdef THREADED
	}
#endif
}

//void Game::drawTestHouse() {
//	for (float x = -18; x < 14; x++) {
//		for (float y = -16; y < 0; y++) {
//			for (float z = -18; z < 14; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = -5; x < -4; x++) {
//		for (float y = 0; y < 3; y++) {
//			for (float z = -5; z < 1; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = 1; x < 2; x++) {
//		for (float y = 0; y < 3; y++) {
//			for (float z = -5; z < 1; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = -5; x < 2; x++) {
//		for (float y = 0; y < 3; y++) {
//			for (float z = 0; z < 1; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = -4; x < 1; x++) {
//		for (float y = 3; y < 4; y++) {
//			for (float z = -4; z < 0; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = -4; x < 1; x++) {
//		for (float y = 2; y < 3; y++) {
//			for (float z = -5; z < -4; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	for (float x = -4; x < 0; x++) {
//		for (float y = 0; y < 1; y++) {
//			for (float z = -5; z < -4; z++) {
//				block.Draw(x * block_render_size, y * block_render_size, z * block_render_size);
//			}
//		}
//	}
//
//	block.Draw(-1.0f * block_render_size, 1.0f * block_render_size, -5.0f * block_render_size);
//}

int Game::start()
{
#ifdef THREADED
	std::thread tr(&Game::doFrame, this);
#endif
	while (1) {
		if (const auto result = Window::processMessages()) {
#ifdef THREADED
			exit = true;
			tr.join();
#endif
			return result.value();
		}
#ifndef THREADED
		doFrame();
#endif

	}
}

Game::~Game() {}