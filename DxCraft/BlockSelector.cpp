#include "BlockSelector.h"

struct BlockSelectorMeshes {
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

	static constexpr float BillBoardModL = 0.8f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 24>> BillBoardL{
		{
			DirectX::XMFLOAT3(-CubeSide * BillBoardModL, -CubeSide, CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(-CubeSide * BillBoardModL, CubeSide, CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(-CubeSide * BillBoardModL, -CubeSide, -CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(-CubeSide * BillBoardModL, CubeSide, -CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(CubeSide* BillBoardModL, -CubeSide, CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(CubeSide* BillBoardModL, CubeSide, CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(CubeSide* BillBoardModL, -CubeSide, -CubeSide * BillBoardModL),
			DirectX::XMFLOAT3(CubeSide* BillBoardModL, CubeSide, -CubeSide * BillBoardModL),
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

	static constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 1> ied =
	{{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }
	}};
};

BlockSelector::BlockSelector(Graphics& gfx, Block::SelectorType type)
	: type(type), renderData(gfx)
{
	renderData.topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	SetType(type, true);
	renderData.CreateVertexShader(L"SelectionVS.cso", BlockSelectorMeshes::ied);
	renderData.CreateIndexBuffer(BlockSelectorMeshes::Cube.second);
	renderData.CreatePixelShader(L"SelectionPS.cso");
}

void BlockSelector::SetType(Block::SelectorType type, bool OverrideCheck)
{
	if (this->type == type && !OverrideCheck) return;

	switch (type) {
	case Block::SelectorType::BLOCK:
	default:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::Cube.first);
		break;
	case Block::SelectorType::BILBOARD_FULL:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::BillBoardL.first);
		break;
	}

	this->type = type;
}

void BlockSelector::SetTransforms(const Transforms& tf)
{
	renderData.UpdateVScBuf(tf);
}

void BlockSelector::Render(Graphics& gfx)
{
	renderData.Render();
}
