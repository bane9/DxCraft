#include "WorldManager.h"
#include "MathFunctions.h"
#include <algorithm>
#include <optional>
#include "Camera.h"
#include <math.h>
#include "BillBoard.h"
#include "EventManager.h"

WorldManager::WorldManager(Graphics& gfx)
	: gfx(gfx), renderData(gfx), wGen(gfx)
{
	Evt::GlobalEvt["wManager"] = this;
	renderData.CreateVertexShader(L"TextureVS.cso", ied);
	renderData.CreatePixelShader(L"TexturePS.cso");
	renderData.Create2DTexture("images\\terrain.png");
}

void WorldManager::CreateChunk(int x, int y, int z, bool empty)
{
	if (y < 0) return;
	wGen.AddNewChunk(chunks[{x, y, z}] = std::make_shared<Chunk>(x, y, z));
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
	if (normalized.x + 1 >= Chunk::ChunkSize) {
		auto neighbourChunk = GetChunkFromBlock(x + 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.x - 1 < 0) {
		auto neighbourChunk = GetChunkFromBlock(x - 1, y, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.y + 1 >= Chunk::ChunkSize) {
		auto neighbourChunk = GetChunkFromBlock(x, y + 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.y - 1 < 0) {
		auto neighbourChunk = GetChunkFromBlock(x, y - 1, z);
		if (neighbourChunk != nullptr) GenerateMesh(neighbourChunk);
	}
	if (normalized.z + 1 >= Chunk::ChunkSize) {
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

void WorldManager::RenderChunks(Camera& cam)
{
	struct TextureTransforms
	{
		DirectX::XMMATRIX modelViewProj;
		/*DirectX::XMMATRIX model;
		DirectX::XMMATRIX transformMatrix;
		DirectX::XMMATRIX projMatrix;*/
	};

	using namespace std::chrono_literals;
	while (creatingChunks) std::this_thread::sleep_for(0.1ms);
	lockThread = true;

	int count = 0;
	for (auto& chunkIt : chunks) {
		if (!chunkIt.second || !chunkIt.second->SafeToAccess || chunkIt.second->IndexBufferSize == 0 ||
			!cam.GetFrustum().IsBoxInFrustum(chunkIt.second->aabb)) continue;
		auto& chunk = chunkIt.second;
		auto model = DirectX::XMMatrixTranslation(chunk->x, chunk->y, chunk->z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			/*DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()*/
		};
		
		renderData.UpdateVScBuf(tf);

		RenderData::Render(renderData, chunk->VertexBuffer, chunk->IndexBuffer,
			chunk->IndexBufferSize, sizeof(Vertex));
	}

	count = 0;
	for (auto& chunkIt : chunks) {
		if (!chunkIt.second || !chunkIt.second->SafeToAccess || chunkIt.second->AdditionalIndexBufferSize == 0 ||
			!cam.GetFrustum().IsBoxInFrustum(chunkIt.second->aabb)) continue;
		auto& chunk = chunkIt.second;
		auto model = DirectX::XMMatrixTranslation(chunk->x, chunk->y, chunk->z);

		const TextureTransforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * cam.GetProjection()),
			/*DirectX::XMMatrixTranspose(model),
			gfx.getCamera() * cam.GetProjection(),
			cam.GetProjection()*/
		};

		renderData.UpdateVScBuf(tf);

		RenderData::Render(renderData, chunk->AdditionalVertexBuffer, chunk->AdditionalIndexBuffer,
			chunk->AdditionalIndexBufferSize, sizeof(Vertex));
	}
	lockThread = false;
}

void WorldManager::UnloadChunks(const Position& pos, float area)
{
	using namespace std::chrono_literals;
	while (lockThread) std::this_thread::sleep_for(0.1ms);
	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		float x = it->first.x, z = it->first.z;
		if (pos.x < x - area || pos.x > x + area ||
			pos.z < z - area || pos.z > z + area) {
			if ((it = chunks.erase(it)) == chunks.end()) break;
		}
	}
}

std::shared_ptr<Chunk> WorldManager::GetChunkFromBlock(int x, int y, int z, bool safetyCheck)
{
	if (y < 0)
		return nullptr;
	using namespace std::chrono_literals;
	while (creatingChunks) std::this_thread::sleep_for(0.1ms);
	lockThread = true;
	Position chunkPosition(
		x - FixedMod(x, Chunk::ChunkSize),
		y - FixedMod(y, Chunk::ChunkSize),
		z - FixedMod(z, Chunk::ChunkSize)
	);

	auto chunk = chunks.find(chunkPosition);
	if (chunk == chunks.end() || !chunk->second || (!chunk->second->SafeToAccess && safetyCheck)) {
		lockThread = false;
		return nullptr;
	}
	else {
		lockThread = false;
		return chunk->second;
	}

}

bool WorldManager::CreateChunkAtPlayerPos(const Position& pos)
{
	using namespace std::chrono_literals;
	while (lockThread) std::this_thread::sleep_for(0.1ms);
	Position chunkPosition(
		(pos.x - FixedMod(pos.x, Chunk::ChunkSize)),
		0,
		(pos.z - FixedMod(pos.z, Chunk::ChunkSize))
	);
	if (chunks.find(chunkPosition) == chunks.end()) {
		for (int i = 0; i < (Chunk::ChunkSize / 16) * 16; i++) {
			CreateChunk(chunkPosition.x, i * Chunk::ChunkSize, chunkPosition.z);
		}
		return true;
	}
	return false;
}

std::shared_ptr<Block> WorldManager::GetBlock(int x, int y, int z, bool safetyCheck)
{
	if (y < 0) return nullptr;
	auto chunk = GetChunkFromBlock(x, y, z, safetyCheck);
		
	if (chunk == nullptr) 
		return nullptr;

	return std::shared_ptr<Block>(chunk, &chunk->operator()(x, y, z));
}

std::shared_ptr<Block> WorldManager::GetBlock(const Position& pos, bool safetyCheck)
{
	return GetBlock(pos.x, pos.y, pos.z, safetyCheck);
}

std::shared_ptr<Block> WorldManager::GetBlock(const DirectX::XMFLOAT3& pos, bool safetyCheck)
{
	return GetBlock(round(pos.x), round(pos.y), round(pos.z), safetyCheck);
}

void WorldManager::GenerateMesh(std::shared_ptr<Chunk> chunkPtr)
{
	wGen.AddChunkForMeshing(chunkPtr);
}
