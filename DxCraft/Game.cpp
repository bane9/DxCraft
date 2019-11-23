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
	: wnd(width, height), wManager(wnd.Gfx()), cameraRay(wManager), 
	renderData(wnd.Gfx(), L"CrosshairVS.cso", L"CrossHairPS.cso", ied),
	blockSelector(wnd.Gfx(), L"SelectionVS.cso", L"SelectionPS.cso", ied)
{
	std::vector<DirectX::XMFLOAT3> tempVertices;
	std::copy(Crosshair::NearSide.first.begin(), Crosshair::NearSide.first.end(), std::back_inserter(tempVertices));

	std::vector<uint16_t> tempIndices;
	std::copy(Crosshair::NearSide.second.begin(), Crosshair::NearSide.second.end(), std::back_inserter(tempIndices));

	auto t = renderData.UpdateVertexBuffer(tempVertices);
	auto t2 = renderData.UpdateIndexBuffer(tempIndices);

	tempVertices.clear();
	std::copy(BlockSelector::Cube.first.begin(), BlockSelector::Cube.first.end(), std::back_inserter(tempVertices));

	tempIndices.clear();
	std::copy(BlockSelector::Cube.second.begin(), BlockSelector::Cube.second.end(), std::back_inserter(tempIndices));

	t = blockSelector.UpdateVertexBuffer(tempVertices);
	t2 = blockSelector.UpdateIndexBuffer(tempIndices);

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

	//wManager.CreateChunk(0, 0, 0);
	//wManager.CreateChunk(0, 1, 0, true);

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


		cameraRay.SetPositionAndDirection(cam.GetPos(), cam.GetPitch(), cam.GetYaw(), 1.75f);
		auto old = cameraRay.GetVector();
		auto n = old;
		bool found = false;

		Position pos(0, 0, 0);

		while (cameraRay.Next()) {
			auto block = wManager.GetBlock(round(n.x), round(n.y), round(n.z));
			if (block != nullptr && block->type != BlockType::Air) {
				pos = { block->x,block->y, block->z };
				found = true;
				break;
			}
			old = std::move(n);
			n = cameraRay.GetVector();
		}

		if (found && !showCursor) {
			if (wnd.mouse.LeftIsPressed() && destroyTimer.getTime() > 0.1f) {
				wManager.ModifyBlock(round(n.x), round(n.y), round(n.z));
				destroyTimer.mark();
			}
			else if (wnd.mouse.RightIsPressed() && placeTimer.getTime() > 0.175f) {
				wManager.ModifyBlock(round(old.x), round(old.y), round(old.z), BlockType::Wooden_Plank);
				placeTimer.mark();
			}
		}
		else {
			destroyTimer.mark();
			placeTimer.mark();
		}

		auto model = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * wnd.Gfx().getCamera() * wnd.Gfx().getProjection()),
			DirectX::XMMatrixTranspose(model)
		};

		blockSelector.UpdateConstantBuffer(tf);
		

		Renderer::Render(wnd.Gfx(), renderData);

		wnd.Gfx().RenderWireframe();
		Renderer::Render(wnd.Gfx(), blockSelector);
		wnd.Gfx().RenderSolid();

		wManager.Draw(wnd.Gfx());

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
