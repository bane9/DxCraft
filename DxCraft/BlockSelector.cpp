#include "BlockSelector.h"

static struct BlockSelectorMeshes {
	static constexpr float CubeSide = 0.501f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 24>> Cube{
		{
			DirectX::XMFLOAT3(-CubeSide, -CubeSide, CubeSide),
			DirectX::XMFLOAT3(-CubeSide, CubeSide, CubeSide),
			DirectX::XMFLOAT3(-CubeSide, -CubeSide, -CubeSide),
			DirectX::XMFLOAT3(-CubeSide, CubeSide, -CubeSide),
			DirectX::XMFLOAT3(CubeSide, -CubeSide, CubeSide),
			DirectX::XMFLOAT3(CubeSide, CubeSide, CubeSide),
			DirectX::XMFLOAT3(CubeSide, -CubeSide, -CubeSide),
			DirectX::XMFLOAT3(CubeSide, CubeSide, -CubeSide),
		},
		{
			0, 1, 3, 
			2, 2, 3, 
			7, 6, 6, 
			7, 5, 4,
			4, 5, 1,
			0, 2, 6, 
			4, 0, 7, 
			3, 1, 5
		}
	};
};

BlockSelector::BlockSelector(Graphics& gfx, Block::SelectorType type)
	: type(type), renderData(gfx)
{
	renderData.topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
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
	renderData.Render();
}
