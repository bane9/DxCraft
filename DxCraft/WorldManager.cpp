#include "WorldManager.h"
#include "MathFunctions.h"
#include <algorithm>
#include <optional>
#include "Camera.h"
#include <math.h>

WorldManager::WorldManager(Graphics& gfx)
	: gfx(gfx), renderData(gfx)
{
	renderData.CreateVertexShader(L"TextureVS.cso", ied);
	renderData.CreatePixelShader(L"TexturePS.cso");
	renderData.Create2DTexture("images\\terrain.png");
}

void WorldManager::CreateChunk(int x, int y, int z, bool empty)
{
	if (y < 0) return;
	chunks.emplace(Position(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize),
		BasicChunk(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize, empty));
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
	Timer t;
	for (auto& chunk : chunks) {
		GenerateMesh(chunk.second);
	}
	auto asd = std::to_wstring(t.Mark() * 1000.0f);
	OutputDebugString(asd.c_str());
}

void WorldManager::DrawOpaque(Camera& cam)
{
	for (auto& chunk : chunks) {
		if(!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * gfx.getProjection()),
			DirectX::XMMatrixTranspose(model)
		};
		
		renderData.UpdateVScBuf(tf);

		if(chunk.second.opaqueIndexBufferSize > 0)
			RenderData::Render(renderData, chunk.second.opaqueVertexBuffer, chunk.second.opaqueIndexBuffer,
				chunk.second.opaqueIndexBufferSize, sizeof(Vertex));
	}
}

void WorldManager::DrawTransparent(Camera& cam)
{
	for (auto& chunk : chunks) {
		if (!cam.GetFrustum().IsBoxInFrustum(chunk.second.aabb)) continue;
		auto model = DirectX::XMMatrixTranslation(chunk.second.x, chunk.second.y, chunk.second.z);

		const Transforms tf =
		{
			DirectX::XMMatrixTranspose(model * gfx.getCamera() * gfx.getProjection()),
			DirectX::XMMatrixTranspose(model)
		};

		renderData.UpdateVScBuf(tf);

		if (chunk.second.transparentIndexBufferSize > 0)
			RenderData::Render(renderData, chunk.second.transparentVertexBuffer, chunk.second.transparentIndexBuffer,
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
		return vertex;
		});
	const int offset = vertexBuffer.size() > 0 ? (vertexBuffer.size() / 4 - 1) * 4 : 0;
	std::transform(face.second.begin(), face.second.end(), std::back_inserter(indexBuffer), [offset](int a) {return offset + a;});
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
					if (BlockVisible(chunk, pos.x + 1, pos.y, pos.z, block.GetBlockType())) {
						AppendFace(Faces::RightSide, transparentVertex, transparentIndex, block.GetTexCoords()[3],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x - 1, pos.y, pos.z, block.GetBlockType())) {
						AppendFace(Faces::LeftSide, transparentVertex, transparentIndex, block.GetTexCoords()[2],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y + 1, pos.z, block.GetBlockType())) {
						AppendFace(Faces::TopSide, transparentVertex, transparentIndex, block.GetTexCoords()[5],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y - 1, pos.z, block.GetBlockType())) {
						AppendFace(Faces::BottomSide, transparentVertex, transparentIndex, block.GetTexCoords()[4],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z + 1, block.GetBlockType())) {
						AppendFace(Faces::FarSide, transparentVertex, transparentIndex, block.GetTexCoords()[0],
							x, y, z);
					}
					if (BlockVisible(chunk, pos.x, pos.y, pos.z - 1, block.GetBlockType())) {
						AppendFace(Faces::NearSide, transparentVertex, transparentIndex, block.GetTexCoords()[1],
							x, y, z);
					}
				}
			}
		}
	}
	
	if (opaqueVertex.size() > 0 && opaqueIndex.size() > 0) {
		chunk.opaqueVertexBuffer = RenderData::CreateVertexBuffer(gfx, opaqueVertex);
		chunk.opaqueIndexBuffer = RenderData::CreateIndexBuffer(gfx, opaqueIndex);
		chunk.opaqueIndexBufferSize = opaqueIndex.size();
	}
	else chunk.opaqueIndexBufferSize = 0;

	if (transparentVertex.size() > 0 && transparentIndex.size() > 0) {
		chunk.transparentVertexBuffer = RenderData::CreateVertexBuffer(gfx, transparentVertex);
		chunk.transparentIndexBuffer = RenderData::CreateIndexBuffer(gfx, transparentIndex);
		chunk.transparentIndexBufferSize = transparentIndex.size();
	}
	else chunk.transparentIndexBufferSize = 0;

}
