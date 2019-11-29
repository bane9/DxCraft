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
private:
	Position Normalize(int x, int y, int z) const noexcept;
	inline int FlatIndex(int x, int y, int z) const noexcept;
	std::vector<Block> blocks;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> opaqueVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> opaqueIndexBuffer;
	UINT opaqueIndexBufferSize = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> transparentVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> transparentIndexBuffer;
	UINT transparentIndexBufferSize = 0;

	int x, y, z;
	AABB aabb;
};
