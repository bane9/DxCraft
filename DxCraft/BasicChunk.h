#pragma once
#include "Block.h"
#include <vector>
#include "XM_Structs.h"
#include <wrl.h>
#include <d3d11.h>
#include "robin_hood.h"
#include "Position.h"


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
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	int x, y, z;
	size_t vertexBufferSize = 0;
	size_t indexBufferSize = 0;
};

