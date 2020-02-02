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
#include <array>

class Chunk
{
	friend class WorldManager;
	friend class WorldGenerator;
	friend class ChunkGenerator;
public:
	Chunk(int x, int y, int z);
	Position GetPosition() const noexcept;
	static constexpr int ChunkSize = 16;

	Block& operator()(int x, int y, int z);
	bool SafeToAccess = false;
	enum class Biome {
		Dirt,
		Grass,
		Sand,
		Stone,

		Biome_count,
		None,
	};
	Biome GetBiome();
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

	std::array<float, ChunkSize * ChunkSize> heightMap;
	bool HasGenerated = false;
	bool OcludedOnNull = false;

	Biome biome = Biome::None;
};
