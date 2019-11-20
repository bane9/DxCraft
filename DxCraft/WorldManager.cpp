#include "WorldManager.h"
#include "MathFunctions.h"
#include <algorithm>

WorldManager::WorldManager(Graphics& gfx)
	: renderer(gfx)
{
}

void WorldManager::CreateChunk(int x, int y, int z)
{
	if (y < 0) return;
	chunks.emplace(Position(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize),
		BasicChunk(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize));
}

void WorldManager::ModifyBlock(int x, int y, int z, BlockType type)
{
	BasicChunk* chunk = GetChunkFromBlock(x, y, z);
	if (chunk == nullptr) return;
	Position normalized = chunk->Normalize(x, y, z);
	Block& block = chunk->blocks[chunk->FlatIndex(x, y, z)];
	if (block.type == BlockType::Bedrock) return;
	block.type = type;
	GenerateMesh(*chunk);
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

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		renderer.Draw(chunk.second);
	}
}

void WorldManager::AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, 
	BasicChunk& chunk, const std::array<float, 2>& texture, float offsetX, float offsetY, float offsetZ)
{
	std::transform(face.first.begin(), face.first.end(), std::back_inserter(chunk.vertices), [offsetX, offsetY, offsetZ, &texture](Vertex vertex) {
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
	const int offset = (chunk.vertices.size() / 4 - 1) * 4;
	std::transform(face.second.begin(), face.second.end(), std::back_inserter(chunk.indices), [offset](int a) {return offset + a; });
}

bool WorldManager::BlockVisible(const BasicChunk& chunk, int x, int y, int z)
{
	if (x < BasicChunk::chunkSize && y < BasicChunk::chunkSize && z < BasicChunk::chunkSize
		&& x > 0 && y > 0 && z > 0)
	{
		if(chunk.blocks[chunk.FlatIndex(x, y, z)].type == BlockType::Air) return true;
	}
	else
	{
		auto block = GetBlock(x, y, z);
		if (block == nullptr) return true;
		return block->type == BlockType::Air;
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
	auto chunk = GetChunkFromBlock(x, y, z);
		
	if (chunk == nullptr) 
		return nullptr;

	return &chunk->blocks[chunk->FlatIndex(x, y, z)];
}

void WorldManager::GenerateMesh(BasicChunk& chunk)
{
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.type == BlockType::Air) continue;

				if(BlockVisible(chunk, block.x + 1, block.y, block.z)) {
					AppendFace(Faces::RightSide, chunk, BlockFaces[static_cast<int>(block.type)][3],
						x, y, z);
				}
				if (BlockVisible(chunk, block.x - 1, block.y, block.z)) {
					AppendFace(Faces::LeftSide, chunk, BlockFaces[static_cast<int>(block.type)][2], 
						x, y, z);
				}
				if (BlockVisible(chunk, block.x, block.y + 1, block.z)) {
					AppendFace(Faces::TopSide, chunk, BlockFaces[static_cast<int>(block.type)][5], 
						x, y, z);
				}
				if (BlockVisible(chunk, block.x, block.y - 1, block.z)) {
					AppendFace(Faces::BottomSide, chunk, BlockFaces[static_cast<int>(block.type)][4], 
						x, y, z);
				}
				if (BlockVisible(chunk, block.x, block.y, block.z + 1)) {
					AppendFace(Faces::FarSide, chunk, BlockFaces[static_cast<int>(block.type)][0],
						x, y, z);
				}
				if (BlockVisible(chunk, block.x, block.y, block.z - 1)) {
					AppendFace(Faces::NearSide, chunk, BlockFaces[static_cast<int>(block.type)][1],
						x, y, z);
				}
			}
		}
	}
	renderer.AppendData(chunk);
}
