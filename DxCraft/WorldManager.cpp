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

void WorldManager::Draw()
{
	for (auto& chunk : chunks) {
		GenerateMesh(chunk.second);
		renderer.Draw(vertices, indices, chunk.second.x, chunk.second.y, chunk.second.z);
	}
}

void WorldManager::AppendFace(const std::pair<std::array<Vertex, 4>, std::array<uint16_t, 6>>& face, float offsetX, float offsetY, float offsetZ)
{
	std::transform(face.first.begin(), face.first.end(), std::back_inserter(vertices), [offsetX, offsetY, offsetZ](Vertex vertex) {
		vertex.pos.x += offsetX * Faces::side * 2;
		vertex.pos.y += offsetY * Faces::side * 2;
		vertex.pos.z += offsetZ * Faces::side * 2;
		return std::move(vertex);
		});
	const int offset = (vertices.size() / 4 - 1) * 4;
	std::transform(face.second.begin(), face.second.end(), std::back_inserter(indices), [offset](int a) {return offset + a; });
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

#define TESTFACE(x) if (x == nullptr || x->type == BlockType::Air)

void WorldManager::GenerateMesh(const BasicChunk& chunk)
{
	vertices.clear();
	indices.clear();
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.type == BlockType::Air) continue;

				TESTFACE(getBlock(block.x + 1, block.y, block.z)) {
					AppendFace(Faces::RightSide, x, y, z);
				}
				TESTFACE(getBlock(block.x - 1, block.y, block.z)) {
					AppendFace(Faces::LeftSide, x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y + 1, block.z)) {
					AppendFace(Faces::TopSide, x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y - 1, block.z)) {
					AppendFace(Faces::BottomSide, x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y, block.z + 1)) {
					AppendFace(Faces::FarSide, x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y, block.z - 1)) {
					AppendFace(Faces::NearSide, x, y, z);
				}
			}
		}
	}
}

bool WorldManager::isVisible(const Block& block)
{
	

	return false;
}
