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
	const auto position = Position(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize);
	chunks.emplace(position,
		BasicChunk(x * BasicChunk::chunkSize, y * BasicChunk::chunkSize, z * BasicChunk::chunkSize));
	const auto& chunk = chunks.find(position);
	GenerateMesh(chunk->second);

}

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		renderer.Draw(chunk.second);
	}
}

void WorldManager::AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, 
	BasicChunk& chunk, float offsetX, float offsetY, float offsetZ)
{
	std::transform(face.first.begin(), face.first.end(), std::back_inserter(chunk.vertices), [offsetX, offsetY, offsetZ](Vertex vertex) {
		vertex.pos.x += offsetX * Faces::side * 2;
		vertex.pos.y += offsetY * Faces::side * 2;
		vertex.pos.z += offsetZ * Faces::side * 2;
		return std::move(vertex);
		});
	const int offset = (chunk.vertices.size() / 4 - 1) * 4;
	std::transform(face.second.begin(), face.second.end(), std::back_inserter(chunk.indices), [offset](int a) {return offset + a; });
}

Block* WorldManager::getBlock(int x, int y, int z)
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

	return &chunk->second.blocks[chunk->second.FlatIndex(x, y, z)];
}

#define TRANSPARENT_BLOCK(x) x.type == BlockType::Air

void WorldManager::GenerateMesh(BasicChunk& chunk)
{
	chunk.vertices.clear();
	chunk.indices.clear();
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.type == BlockType::Air) continue;
				if (x + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x + 1, y, z)])) {
					AppendFace(Faces::RightSide, chunk, x, y, z);
				}
				if (x - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x - 1, y, z)])) {
					AppendFace(Faces::LeftSide, chunk, x, y, z);
				}
				if (y + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y + 1, z)])) {
					AppendFace(Faces::TopSide, chunk, x, y, z);
				}
				if (y - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y - 1, z)])) {
					AppendFace(Faces::BottomSide, chunk, x, y, z);
				}
				if (z + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y, z + 1)])) {
					AppendFace(Faces::FarSide, chunk, x, y, z);
				}
				if (z - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y, z - 1)])) {
					AppendFace(Faces::NearSide, chunk, x, y, z);
				}
			}
		}
	}
}
