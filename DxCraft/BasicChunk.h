#pragma once
#include "Block.h"
#include <vector>
#include "XM_Structs.h"
#include <wrl.h>
#include <d3d11.h>
#include "robin_hood.h"
#include "Position.h"
#include "AABB.h"
#include "RenderData.h"

class BasicChunk
{
	friend class WorldManager;
	friend class MeshRenderer;
public:
	BasicChunk(int x, int y, int z, bool empty = false);
	Position GetPosition() const noexcept;
	static constexpr int chunkSize = 16;
public:
	Position Normalize(int x, int y, int z) const noexcept;
	inline int FlatIndex(int x, int y, int z) const noexcept;
	std::vector<Block> blocks;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	UINT IndexBufferSize = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> AdditionalVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> AdditionalIndexBuffer;
	UINT AdditionalIndexBufferSize = 0;

	int x, y, z;
	AABB aabb;
};
