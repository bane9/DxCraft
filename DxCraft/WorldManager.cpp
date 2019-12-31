#include "WorldManager.h"
#include "MathFunctions.h"
#include <algorithm>
#include "Renderer.h"
#include <optional>
#include "Camera.h"
#include "RenderDataFactory.h"
#include <math.h>

WorldManager::WorldManager(Graphics& gfx)
	: gfx(gfx)
{
	RenderDataFactory::CreateVertexShader(gfx, renderData, L"TextureVS.cso", ied);
	RenderDataFactory::CreatePixelShader(gfx, renderData, L"TexturePS.cso");
	RenderDataFactory::Create2DTexture(gfx, renderData, "images\\terrain.png");
}

void WorldManager::CreateChunk(int x, int y, int z, bool empty)
{
	if (y < 0) return;
	Position pos(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize);
	chunks.emplace(pos, BasicChunk(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize, empty));
}

void WorldManager::ModifyBlock(int x, int y, int z, Block::BlockType type)
{
	if (y < 0) return;
	BasicChunk* chunk = GetChunkFromBlock(x, y, z);
	if (chunk == nullptr) return;
	Position normalized = chunk->Normalize(x, y, z);
	Block& block = chunk->blocks[chunk->FlatIndex(x, y, z)];
	if (block.GetBlockType() == Block::BlockType::Bedrock) return;
	block.SetBlockType(type);
	GenerateMesh(*chunk);
	if (block.IsTransparent() && block.GetBlockType() != Block::BlockType::Air) return;
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

}

void WorldManager::GenerateMeshes() {
	for (auto& chunk : chunks) {
		GenerateMesh(chunk.second);
	}
}

void WorldManager::DrawOpaque(Graphics& gfx, Camera& cam)
{
	for (auto& chunk : chunks) {
		if(!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * gfx.getProjection()),
			DirectX::XMMatrixTranspose(model)
		};
		
		RenderDataFactory::UpdateVScBuf(gfx, renderData, tf);

		if(chunk.second.opaqueIndexBufferSize > 0)
			Renderer::DrawIndexed(gfx, renderData, chunk.second.opaqueVertexBuffer, chunk.second.opaqueIndexBuffer,
				chunk.second.opaqueIndexBufferSize, sizeof(Vertex));
	}
}

void WorldManager::DrawTransparent(Graphics& gfx, Camera& cam)
{
	for (auto& chunk : chunks) {
		if (!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * gfx.getProjection()),
			DirectX::XMMatrixTranspose(model)
		};

		RenderDataFactory::UpdateVScBuf(gfx, renderData, tf);

		if (chunk.second.transparentIndexBufferSize > 0)
			Renderer::DrawIndexed(gfx, renderData, chunk.second.transparentVertexBuffer, chunk.second.transparentIndexBuffer,
				chunk.second.transparentIndexBufferSize, sizeof(Vertex));
	}
}

void WorldManager::AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face,
	std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
	const std::array<float, 2>& texture, float offsetX, float offsetY, float offsetZ)
{
	std::transform(face.first.begin(), face.first.end(), std::back_inserter(vertexBuffer), [offsetX, offsetY, offsetZ, &texture](Vertex vertex) {
		vertex.pos.x += offsetX;
		vertex.pos.y += offsetY;
		vertex.pos.z += offsetZ;
		const float startTexX = texture[0] / 16.0f;
		const float endTexX   = (texture[0] + 1.0f) / 16.0f;
		const float startTexY = texture[1] / 16.0f;
		const float endTexY   = (texture[1] + 1.0f) / 16.0f;
		vertex.tc.x = vertex.tc.x == 1 ? startTexX : endTexX;
		vertex.tc.y = vertex.tc.y == 1 ? startTexY : endTexY;
		return std::move(vertex);
		});
	const int offset = vertexBuffer.size() > 0 ? (vertexBuffer.size() / 4 - 1) * 4 : 0;
	std::transform(face.second.begin(), face.second.end(), std::back_inserter(indexBuffer), [offset](int a) {return offset + a;});
}

bool WorldManager::BlockVisible(const BasicChunk& chunk, int x, int y, int z)
{
	if (y < 0) return true;
	if (x < BasicChunk::chunkSize - 1 && y < BasicChunk::chunkSize - 1 && z < BasicChunk::chunkSize - 1
		&& x > 0 && y > 0 && z > 0)
	{
		return chunk.blocks[chunk.FlatIndex(x, y, z)].IsTransparent();
	}
	else
	{
		auto block = GetBlock(x, y, z);
		if (block == nullptr) return true;
		return block->IsTransparent();
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

Block* WorldManager::GetBlock(int x, int y, int z)
{
	if (y < 0) return nullptr;
	auto chunk = GetChunkFromBlock(x, y, z);
		
	if (chunk == nullptr) 
		return nullptr;

	return &chunk->blocks[chunk->FlatIndex(x, y, z)];
}

Block* WorldManager::GetBlock(const DirectX::XMFLOAT3& pos)
{
	return GetBlock(round(pos.x), round(pos.y), round(pos.z));
}

void WorldManager::GenerateMesh(BasicChunk& chunk)
{
	std::vector<Vertex> opaqueVertex;
	std::vector<uint16_t> opaqueIndex;

	std::vector<Vertex> transparentVertex;
	std::vector<uint16_t> transparentIndex;

	int numOfAirBlocks = 0;
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.GetBlockType() == Block::BlockType::Air) continue;

				auto pos = block.GetPosition();

				if (!block.IsTransparent()) {
					if (BlockVisible(chunk, pos.x + 1, pos.y, pos.z)) {
						AppendFace(Faces::RightSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x - 1, pos.y, pos.z)) {
						AppendFace(Faces::LeftSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y + 1, pos.z)) {
						AppendFace(Faces::TopSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y - 1, pos.z)) {
						AppendFace(Faces::BottomSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z + 1)) {
						AppendFace(Faces::FarSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z - 1)) {
						AppendFace(Faces::NearSide, opaqueVertex, opaqueIndex, block.GetTexCoords()[1],
							x, y, z);
					}
				}
				else {
					if (BlockVisible(chunk, pos.x + 1, pos.y, pos.z)) {
						AppendFace(Faces::RightSide, transparentVertex, transparentIndex, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x - 1, pos.y, pos.z)) {
						AppendFace(Faces::LeftSide, transparentVertex, transparentIndex, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y + 1, pos.z)) {
						AppendFace(Faces::TopSide, transparentVertex, transparentIndex, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y - 1, pos.z)) {
						AppendFace(Faces::BottomSide, transparentVertex, transparentIndex, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z + 1)) {
						AppendFace(Faces::FarSide, transparentVertex, transparentIndex, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z - 1)) {
						AppendFace(Faces::NearSide, transparentVertex, transparentIndex, block.GetTexCoords()[1],
							x, y, z);
					}
				}
			}
		}
	}
	
	if (opaqueVertex.size() > 0 && opaqueIndex.size() > 0) {
		chunk.opaqueVertexBuffer = RenderDataFactory::CreateVertexBuffer(gfx, opaqueVertex);
		chunk.opaqueIndexBuffer = RenderDataFactory::CreateIndexBuffer(gfx, opaqueIndex);
		chunk.opaqueIndexBufferSize = opaqueIndex.size();
	}
	else chunk.opaqueIndexBufferSize = 0;

	if (transparentVertex.size() > 0 && transparentIndex.size() > 0) {
		chunk.transparentVertexBuffer = RenderDataFactory::CreateVertexBuffer(gfx, transparentVertex);
		chunk.transparentIndexBuffer = RenderDataFactory::CreateIndexBuffer(gfx, transparentIndex);
		chunk.transparentIndexBufferSize = transparentIndex.size();
	}
	else chunk.transparentIndexBufferSize = 0;

}
