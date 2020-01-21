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

std::shared_ptr<BasicChunk> WorldManager::CreateChunk(int x, int y, int z, bool empty)
{
	if (y < 0) return nullptr;
	return chunks[{x, y, z}] = std::make_shared<BasicChunk>(x, y, z, empty);
}

bool WorldManager::ModifyBlock(int x, int y, int z, Block::BlockType type)
{
	if (y < 0) return false;
	auto chunk = GetChunkFromBlock(x, y, z);
	if (chunk == nullptr) return false;
	Position normalized = chunk->Normalize(x, y, z);
	Block& block = chunk->blocks[chunk->FlatIndex(x, y, z)];
	if (block.GetBlockType() == Block::BlockType::Bedrock) return false;
	block.SetBlockType(type);
	GenerateMesh(chunk);
	if (block.GetBlockType() != Block::BlockType::Air) return true;
	if (normalized.x + 1 >= BasicChunk::chunkSize) {
		auto neighbourChunk = GetChunkFromBlock(x + 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.x - 1 < 0) {
		auto neighbourChunk = GetChunkFromBlock(x - 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.y + 1 >= BasicChunk::chunkSize) {
		auto neighbourChunk = GetChunkFromBlock(x, y + 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.y - 1 < 0) {
		auto neighbourChunk = GetChunkFromBlock(x, y - 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.z + 1 >= BasicChunk::chunkSize) {
		auto neighbourChunk = GetChunkFromBlock(x, y, z + 1);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.z - 1 < 0) {
		auto neighbourChunk = GetChunkFromBlock(x, y, z - 1);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
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

	for (auto& chunkIt : chunks) {
		auto chunk = chunkIt.second;
		if (chunk == nullptr) continue;
		if(!cam.GetFrustum().IsBoxInFrustum(chunk->aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk->x, chunk->y, chunk->z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()
		};
		
		renderData.UpdateVScBuf(tf);

		if(chunk->IndexBufferSize > 0)
			RenderData::Render(renderData, chunk->VertexBuffer, chunk->IndexBuffer,
				chunk->IndexBufferSize, sizeof(Vertex));
	}

	for (auto& chunkIt : chunks) {
		auto chunk = chunkIt.second;
		if (chunk == nullptr) continue;
		if (!cam.GetFrustum().IsBoxInFrustum(chunk->aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk->x, chunk->y, chunk->z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()
		};

		renderData.UpdateVScBuf(tf);

		if (chunk->AdditionalIndexBufferSize > 0)
			RenderData::Render(renderData, chunk->AdditionalVertexBuffer, chunk->AdditionalIndexBuffer,
				chunk->AdditionalIndexBufferSize, sizeof(Vertex));
	}
}

void WorldManager::UnloadChunks(const Position& pos, float area)
{
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		float x = (*it).first.x, z = (*it).first.z;
		if (pos.x < x - area || pos.x > x + area ||
			pos.z < z - area || pos.z > z + area) {
			if ((it = chunks.erase(it)) == chunks.end()) break;
		}
	}
}

bool WorldManager::BlockVisible(std::shared_ptr<BasicChunk> chunkPtr, int x, int y, int z, Block::BlockType type)
{
	if (chunkPtr == nullptr) return true;
	auto& chunk = *chunkPtr;
	if (y < 0) return true;
	if (x < BasicChunk::chunkSize - 1 && y < BasicChunk::chunkSize - 1 && z < BasicChunk::chunkSize - 1
		&& x > 0 && y > 0 && z > 0)
	{
		auto block = chunk(x, y, z);
		return block.IsTransparent() && block.GetBlockType() != type;
	}
	else
	{
		auto chunk = GetChunkFromBlock(x, y, z);
		if (chunk == nullptr) return true;
		auto& block = chunk->operator()(x, y, z);
		return block.IsTransparent() && block.GetBlockType() != type;
	}
	return false;
}

std::shared_ptr<BasicChunk> WorldManager::GetChunkFromBlock(int x, int y, int z)
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
	else return chunk->second;
}

std::optional<std::vector<std::shared_ptr<BasicChunk>>> WorldManager::CreateChunkAtPlayerPos(const Position& pos)
{
	Position chunkPosition(
		(pos.x - FixedMod(pos.x, BasicChunk::chunkSize)),
		0,
		(pos.z - FixedMod(pos.z, BasicChunk::chunkSize))
	);
	std::vector<std::shared_ptr<BasicChunk>> out(16);
	if (chunks.find(chunkPosition) == chunks.end()) {
		for (int i = 0; i < 16; i++) {
			out[i] = CreateChunk(chunkPosition.x, i * 16, chunkPosition.z);
		}
		return out;
	}
	return {};
}

std::shared_ptr<Block> WorldManager::GetBlock(int x, int y, int z)
{
	if (y < 0) return nullptr;
	auto chunk = GetChunkFromBlock(x, y, z);
		
	if (chunk == nullptr) 
		return nullptr;

	return std::shared_ptr<Block>(chunk, &chunk->operator()(x, y, z));
}

std::shared_ptr<Block> WorldManager::GetBlock(const Position& pos)
{
	return GetBlock(pos.x, pos.y, pos.z);
}

std::shared_ptr<Block> WorldManager::GetBlock(const DirectX::XMFLOAT3& pos)
{
	return GetBlock(round(pos.x), round(pos.y), round(pos.z));
}

void WorldManager::GenerateMesh(std::shared_ptr<BasicChunk> chunkPtr)
{
	if (chunkPtr == nullptr) return;
	BasicChunk& chunk = *chunkPtr;
	std::vector<Vertex> VertexBuffer;
	std::vector<uint16_t> IndexBuffer;

	std::vector<Vertex> AdditionalVertexBuffer;
	std::vector<uint16_t> AdditionalIndexBuffer;

	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk(x, y, z);
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
					if (BlockVisible(chunkPtr, pos.x + 1, pos.y, pos.z)) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x - 1, pos.y, pos.z)) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y + 1, pos.z)) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y - 1, pos.z)) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y, pos.z + 1)) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y, pos.z - 1)) {
						AppendMesh(Faces::NearSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[1],
							x, y, z);
					}
				}
				else {
					if (BlockVisible(chunkPtr, pos.x + 1, pos.y, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x - 1, pos.y, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y + 1, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y - 1, pos.z, block.GetBlockType())) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y, pos.z + 1, block.GetBlockType())) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunkPtr, pos.x, pos.y, pos.z - 1, block.GetBlockType())) {
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
