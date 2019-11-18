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

void WorldManager::GenerateMeshes() {
	for (auto& chunk : chunks) {
		GenerateMesh(chunk.second);
		renderer.AppendData(chunk.second);
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
		vertex.pos.x += offsetX * Faces::side * 2;
		vertex.pos.y += offsetY * Faces::side * 2;
		vertex.pos.z += offsetZ * Faces::side * 2;
		float startTexX = texture[0] / 16.0f;
		float endTexX   = (texture[0] + 1.0f) / 16.0f;
		float startTexY = texture[1] / 16.0f;
		float endTexY   = (texture[1] + 1.0f) / 16.0f;
		vertex.tc.x = vertex.tc.x == 1 ? startTexX : endTexX;
		vertex.tc.y = vertex.tc.y == 1 ? startTexY : endTexY;
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

#define NEIGHBOUR_CHECKING

void WorldManager::GenerateMesh(BasicChunk& chunk)
{
	chunk.vertices.clear();
	chunk.indices.clear();
#ifdef NEIGHBOUR_CHECKING
#define TESTFACE(x) if (x == nullptr || x->type == BlockType::Air)
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.type == BlockType::Air) continue;

				TESTFACE(getBlock(block.x + 1, block.y, block.z)) {
					AppendFace(Faces::RightSide, chunk, BlockFaces[static_cast<int>(block.type)][3],
						x, y, z);
				}
				TESTFACE(getBlock(block.x - 1, block.y, block.z)) {
					AppendFace(Faces::LeftSide, chunk, BlockFaces[static_cast<int>(block.type)][2], 
						x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y + 1, block.z)) {
					AppendFace(Faces::TopSide, chunk, BlockFaces[static_cast<int>(block.type)][5], 
						x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y - 1, block.z)) {
					AppendFace(Faces::BottomSide, chunk, BlockFaces[static_cast<int>(block.type)][4], 
						x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y, block.z + 1)) {
					AppendFace(Faces::FarSide, chunk, BlockFaces[static_cast<int>(block.type)][0],
						x, y, z);
				}
				TESTFACE(getBlock(block.x, block.y, block.z - 1)) {
					AppendFace(Faces::NearSide, chunk, BlockFaces[static_cast<int>(block.type)][1],
						x, y, z);
				}
			}
		}
	}
#else
#define TRANSPARENT_BLOCK(x) x.type == BlockType::Air
	for (int x = 0; x < BasicChunk::chunkSize; x++) {
		for (int y = 0; y < BasicChunk::chunkSize; y++) {
			for (int z = 0; z < BasicChunk::chunkSize; z++) {
				const Block& block = chunk.blocks[chunk.FlatIndex(x, y, z)];
				if (block.type == BlockType::Air) continue;
				if (x + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x + 1, y, z)])) {
					AppendFace(Faces::RightSide, chunk, BlockFaces[static_cast<int>(block.type)][0], 
						x, y, z);
				}
				if (x - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x - 1, y, z)])) {
					AppendFace(Faces::LeftSide, chunk, BlockFaces[static_cast<int>(block.type)][0], 
						x, y, z);
				}
				if (y + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y + 1, z)])) {
					AppendFace(Faces::TopSide, chunk, BlockFaces[static_cast<int>(block.type)][0], 
						x, y, z);
				}
				if (y - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y - 1, z)])) {
					AppendFace(Faces::BottomSide, chunk, BlockFaces[static_cast<int>(block.type)][0], 
						x, y, z);
				}
				if (z + 1 == BasicChunk::chunkSize || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y, z + 1)])) {
					AppendFace(Faces::FarSide, chunk, BlockFaces[static_cast<int>(block.type)][0],
						x, y, z);
				}
				if (z - 1 < 0 || TRANSPARENT_BLOCK(chunk.blocks[chunk.FlatIndex(x, y, z - 1)])) {
					AppendFace(Faces::NearSide, chunk, BlockFaces[static_cast<int>(block.type)][0],
						x, y, z);
				}
			}
		}
	}
#endif
}
