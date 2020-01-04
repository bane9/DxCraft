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

	static constexpr float MushroomOffset = 0.4f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 24>> Mushroom{
		{
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, -CubeSide * MushroomOffset, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, CubeSide * MushroomOffset, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, -CubeSide * MushroomOffset, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, CubeSide * MushroomOffset, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, -CubeSide * MushroomOffset, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, CubeSide * MushroomOffset, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, -CubeSide * MushroomOffset, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, CubeSide * MushroomOffset, -CubeSide * MushroomOffset),
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
	
	static constexpr float FlowerY = 0.35f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 24>> Flower{
		{
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, -FlowerY, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset,  FlowerY, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset, -FlowerY, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(-CubeSide * MushroomOffset,  FlowerY, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, -FlowerY, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset,  FlowerY, CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset, -FlowerY, -CubeSide * MushroomOffset),
			DirectX::XMFLOAT3(CubeSide * MushroomOffset,  FlowerY, -CubeSide * MushroomOffset),
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

	static constexpr float SaplingSide = 0.3f;
	static constexpr std::pair<std::array<DirectX::XMFLOAT3, 8>, std::array<uint16_t, 24>> Sapling{
		{
			DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, SaplingSide),
			DirectX::XMFLOAT3(-SaplingSide, SaplingSide, SaplingSide),
			DirectX::XMFLOAT3(-SaplingSide, -SaplingSide, -SaplingSide),
			DirectX::XMFLOAT3(-SaplingSide, SaplingSide, -SaplingSide),
			DirectX::XMFLOAT3(SaplingSide, -SaplingSide, SaplingSide),
			DirectX::XMFLOAT3(SaplingSide, SaplingSide, SaplingSide),
			DirectX::XMFLOAT3(SaplingSide, -SaplingSide, -SaplingSide),
			DirectX::XMFLOAT3(SaplingSide, SaplingSide, -SaplingSide),
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
	case Block::SelectorType::BILBOARD_FULL_L:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::BillBoardL.first);
		break;
	case Block::SelectorType::MUSHROOM:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::Mushroom.first);
		break;
	case Block::SelectorType::FLOWER:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::Flower.first);
		break;
	case Block::SelectorType::SAPLING:
		renderData.CreateVertexBuffer(BlockSelectorMeshes::Sapling.first);
		break;
	}

	this->type = type;
}

void BlockSelector::SetPosition(Position pos, DirectX::XMMATRIX cam, DirectX::XMMATRIX projection)
{
	float yOffset = 0.0f;
	switch (type) {
	case Block::SelectorType::BLOCK:
	case Block::SelectorType::BILBOARD_FULL_L:
	default:
		yOffset = BlockSelectorMeshes::CubeSide;
		break;
	case Block::SelectorType::MUSHROOM:
		yOffset = BlockSelectorMeshes::CubeSide * BlockSelectorMeshes::MushroomOffset;
		break;
	case Block::SelectorType::FLOWER:
		yOffset = BlockSelectorMeshes::FlowerY;
		break;
	case Block::SelectorType::SAPLING:
		yOffset = BlockSelectorMeshes::SaplingSide;
		break;
	}
	auto model = DirectX::XMMatrixTranslation(pos.x, pos.y - (0.5f - yOffset), pos.z);

	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model * cam * projection),
		DirectX::XMMatrixTranspose(model)
	};

	renderData.UpdateVScBuf(tf);
}

void BlockSelector::Render(Graphics& gfx)
{
	renderData.Render();
}
