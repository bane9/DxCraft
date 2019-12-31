#include "BlockSelector.h"
#include "RenderDataFactory.h"
#include "Renderer.h"

static struct BlockSelectorMeshes {
	static constexpr float side = 0.5008f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 36>> Cube{
		{
			DirectX::XMFLOAT3(-side,-side,-side),
			DirectX::XMFLOAT3(side,-side,-side),
			DirectX::XMFLOAT3(-side,side,-side),
			DirectX::XMFLOAT3(side,side,-side),
			DirectX::XMFLOAT3(-side,-side,side),
			DirectX::XMFLOAT3(side,-side,side),
			DirectX::XMFLOAT3(-side,side,side),
			DirectX::XMFLOAT3(side,side,side)
		},
		{
			0, 2, 1, 2, 3, 1,
			1, 3, 5, 3, 7, 5,
			2, 6, 3, 3, 6, 7,
			4, 5, 7, 4, 7, 6,
			0, 4, 2, 2, 4, 6,
			0, 1, 4, 1, 5, 4
		}
	};
};

BlockSelector::BlockSelector(Graphics& gfx, Block::SelectorType type)
	: type(type)
{
	SetType(gfx, type, true);
}

void BlockSelector::SetType(Graphics& gfx, Block::SelectorType type, bool OverrideCheck)
{
	if (this->type == type && !OverrideCheck) return;

	switch (type) {
	case Block::SelectorType::BLOCK:
	default:
		RenderDataFactory::CreateVertexBuffer(gfx, renderData, BlockSelectorMeshes::Cube.first);
		RenderDataFactory::CreateIndexBuffer(gfx, renderData, BlockSelectorMeshes::Cube.second);
		break;
	}

	RenderDataFactory::CreateVertexShader(gfx, renderData, L"SelectionVS.cso");
	RenderDataFactory::CreatePixelShader(gfx, renderData, L"SelectionPS.cso");
}

void BlockSelector::SetTransforms(Graphics& gfx, const Transforms& tf)
{
	RenderDataFactory::UpdateVScBuf(gfx, renderData, tf);
}

void BlockSelector::Render(Graphics& gfx)
{
	gfx.RenderWireframe();
	Renderer::DrawIndexed(gfx, renderData);
	gfx.RenderSolid();
}

