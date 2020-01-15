#include "WorldManager.h"
#include "MathFunctions.h"
#include <algorithm>
#include <optional>
#include "Camera.h"
#include <math.h>
#include "BillBoard.h"

WorldManager::WorldManager(Graphics& gfx)
	: gfx(gfx), renderData(gfx)
{
	renderData.CreateVertexShader(L"TextureVS.cso", ied);
	renderData.CreatePixelShader(L"TexturePS.cso");
	renderData.Create2DTexture("images\\terrain.png");
}

BasicChunk* WorldManager::CreateChunk(int x, int y, int z, bool empty)
{
	if (y < 0) return nullptr;
	auto emplaced = chunks.emplace(Position(x, y, z), BasicChunk(x, y, z, empty));
	if (emplaced.second == false) return nullptr;
	return &(*emplaced.first).second;
}

bool WorldManager::ModifyBlock(int x, int y, int z, Block::BlockType type)
{
	if (y < 0) return false;
	BasicChunk* chunk = GetChunkFromBlock(x, y, z);
	if (chunk == nullptr) return false;
	Position normalized = chunk->Normalize(x, y, z);
	Block& block = chunk->blocks[chunk->FlatIndex(x, y, z)];
	if (block.GetBlockType() == Block::BlockType::Bedrock) return false;
	block.SetBlockType(type);
	GenerateMesh(*chunk);
	if (block.GetBlockType() != Block::BlockType::Air) return true;
	if (normalized.x + 1 >= BasicChunk::chunkSize) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x + 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	if (normalized.x - 1 < 0) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x - 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	if (normalized.y + 1 >= BasicChunk::chunkSize) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x, y + 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	if (normalized.y - 1 < 0) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x, y - 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	if (normalized.z + 1 >= BasicChunk::chunkSize) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x, y, z + 1);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	if (normalized.z - 1 < 0) {
		BasicChunk* neighbourChunk = GetChunkFromBlock(x, y, z - 1);
		if (neighbourChunk != nullptr) GenerateMesh(*neighbourChunk);
	}
	return true;
}

bool WorldManager::ModifyBlock(const Position& pos, Block::BlockType type)
{
	return ModifyBlock(pos.x, pos.y, pos.z, type);
}

void WorldManager::GenerateMeshes() {
	for (auto& chunk : chunks) {
		GenerateMesh(chunk.second);
	}
}

void WorldManager::RenderChunks(Camera& cam)
{
	struct TextureTransforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX transformMatrix;
		DirectX::XMMATRIX projMatrix;
	};

	for (auto& chunk : chunks) {
		if(!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()
		};
		
		renderData.UpdateVScBuf(tf);

		if(chunk.second.IndexBufferSize > 0)
			RenderData::Render(renderData, chunk.second.VertexBuffer, chunk.second.IndexBuffer,
				chunk.second.IndexBufferSize, sizeof(Vertex));
	}

	for (auto& chunk : chunks) {
		if (!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()
		};

		renderData.UpdateVScBuf(tf);

		if (chunk.second.AdditionalIndexBufferSize > 0)
			RenderData::Render(renderData, chunk.second.AdditionalVertexBuffer, chunk.second.AdditionalIndexBuffer,
				chunk.second.AdditionalIndexBufferSize, sizeof(Vertex));
	}
}

void WorldManager::UnloadChunks(const Position& pos, int area)
{
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		if (abs(PointDistance3D(Position((*it).first.x, 0, (*it).first.z), Position(pos.x, 0, pos.z))) > area) {
			if ((it = chunks.erase(it)) == chunks.end()) break;
		}
	}
}

bool WorldManager::BlockVisible(const BasicChunk& chunk, int x, int y, int z, Block::BlockType type)
{
	if (y < 0) return true;
	if (x < BasicChunk::chunkSize - 1 && y < BasicChunk::chunkSize - 1 && z < BasicChunk::chunkSize - 1
		&& x > 0 && y > 0 && z > 0)
	{
		auto block = chunk.blocks[chunk.FlatIndex(x, y, z)];
		return block.IsTransparent() && block.GetBlockType() != type;
	}
	else
	{
		auto block = GetBlock(x, y, z);
		if (block == nullptr) return true;
		return block->IsTransparent() && block->GetBlockType() != type;
	}
	return false;
}

BasicChunk* WorldManager::GetChunkFromBlock(int x, int y, int z)
{
	if (y < 0)
		return nullptr;

	Position chunkPosition(
		x - FixedMod(x, BasicChunk::chunkSize),
		y - FixedMod(y, BasicChunk::chunkSize),
		z - FixedMod(z, BasicChunk::chunkSize)
	);
	
	auto chunk = chunks.find(chunkPosition);

	if (chunk == chunks.end())
		return nullptr;
	else return &chunk->second;
}

std::optional<std::vector<BasicChunk*>> WorldManager::CreateChunkAtPlayerPos(const Position& pos)
{
	Position chunkPosition(
		(pos.x - FixedMod(pos.x, BasicChunk::chunkSize)),
		0,
		(pos.z - FixedMod(pos.z, BasicChunk::chunkSize))
	);
	std::vector<BasicChunk*> out(16);
	if (chunks.find(chunkPosition) == chunks.end()) {
		for (int i = 0; i < 16; i++) {
			CreateChunk(chunkPosition.x, i * 16, chunkPosition.z);
		}
		for (int i = 0; i < 16; i++) {
			out[i] = &chunks.at(Position(chunkPosition.x, i * 16, chunkPosition.z));
		}
		return out;
	}
	return {};
}

Block* WorldManager::GetBlock(int x, int y, int z)
{
	if (y < 0) return nullptr;
	auto chunk = GetChunkFromBlock(x, y, z);
		
	if (chunk == nullptr) 
		return nullptr;

	return &chunk->blocks[chunk->FlatIndex(x, y, z)];
}

Block* WorldManager::GetBlock(const Position& pos)
{
	return GetBlock(pos.x, pos.y, pos.z);
}

Block* WorldManager::GetBlock(const DirectX::XMFLOAT3& pos)
{
	return GetBlock(round(pos.x), round(pos.y), round(pos.z));
}

void WorldManager::GenerateMesh(BasicChunk& chunk)
{
	std::vector<Vertex> VertexBuffer;
	std::vector<uint16_t> IndexBuffer;

	std::vector<Vertex> AdditionalVertexBuffer;
	std::vector<uint16_t> AdditionalIndexBuffer;

	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.GetBlockType() == Block::BlockType::Air) continue;
				auto pos = block.GetPosition();

				auto& TargetVertexBuffer = block.NeedsSeperateDrawCall() ? AdditionalVertexBuffer : VertexBuffer;
				auto& TargetIndexBuffer = block.NeedsSeperateDrawCall() ? AdditionalIndexBuffer : IndexBuffer;

				switch(block.GetMeshType()){
				case Block::MeshType::FULL_MESH_L:
					AppendMesh(BillBoard::MeshL, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords(), x, y, z);
					continue;
				case Block::MeshType::FULL_MESH_S:
					AppendMesh(BillBoard::MeshS, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords(), x, y, z);
					continue;
				case Block::MeshType::SAPLING:
					AppendMesh(BillBoard::Sapling, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords(), x, 
						y - (0.5f - BillBoard::SaplingSideY), z);
					continue;
				default:
					break;
				}

				if (!block.IsTransparent()) {
					if (BlockVisible(chunk, pos.x + 1, pos.y, pos.z)) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x - 1, pos.y, pos.z)) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y + 1, pos.z)) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y - 1, pos.z)) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z + 1)) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z - 1)) {
						AppendMesh(Faces::NearSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[1],
							x, y, z);
					}
				}
				else {
					if (BlockVisible(chunk, pos.x + 1, pos.y, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x - 1, pos.y, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y + 1, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y - 1, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z + 1, block.GetBlockType())) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z - 1, block.GetBlockType())) {
						AppendMesh(Faces::NearSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[1],
							x, y, z);
					}
				}
			}
		}
	}
	
	if (VertexBuffer.size() > 0 && IndexBuffer.size() > 0) {
		chunk.VertexBuffer = RenderData::CreateVertexBuffer(gfx, VertexBuffer);
		chunk.IndexBuffer = RenderData::CreateIndexBuffer(gfx, IndexBuffer);
		chunk.IndexBufferSize = IndexBuffer.size();
	}
	else chunk.IndexBufferSize = 0;

	if (AdditionalVertexBuffer.size() > 0 && AdditionalIndexBuffer.size() > 0) {
		chunk.AdditionalVertexBuffer = RenderData::CreateVertexBuffer(gfx, AdditionalVertexBuffer);
		chunk.AdditionalIndexBuffer = RenderData::CreateIndexBuffer(gfx, AdditionalIndexBuffer);
		chunk.AdditionalIndexBufferSize = AdditionalIndexBuffer.size();
	}
	else chunk.AdditionalIndexBufferSize = 0;

}
