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

class Chunk
{
	friend class WorldManager;
	friend class WorldGenerator;
public:
	Chunk(int x, int y, int z, bool empty = false);
	Position GetPosition() const noexcept;
	static constexpr int ChunkSize = 16;

	Block& operator()(int x, int y, int z);
	bool SafeToWrite = false;
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
