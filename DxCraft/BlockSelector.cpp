#include "BlockSelector.h"

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
	: type(type), renderData(gfx)
{
	SetType(type, true);
}

void BlockSelector::SetType(Block::SelectorType type, bool OverrideCheck)
{
	if (this->type == type && !OverrideCheck) return;

	switch (type) {
	case Block::SelectorType::BLOCK:
	default:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::Cube.first);
		renderData.CreateIndexBuffer(BlockSelectorMeshes::Cube.second);
		break;
	}

	renderData.CreateVertexShader(L"SelectionVS.cso");
	renderData.CreatePixelShader(L"SelectionPS.cso");
}

void BlockSelector::SetTransforms(const Transforms& tf)
{
	renderData.UpdateVScBuf(tf);
}

void BlockSelector::Render(Graphics& gfx)
{
	gfx.RenderWireframe();
	renderData.Render();
	gfx.RenderSolid();
}

