#include "WorldGenerator.h"
#include "EventManager.h"
#include "WorldManager.h"
#include "BillBoard.h"
#include <algorithm>
#include <execution>
#include "ChunkGenerator.h"
#include <bitset>

static auto GetChunkFromBlock = [&](int x, int y, int z) {
	static WorldManager& wManager = *((WorldManager*)Evt::GlobalEvt["wManager"]);
	return wManager.GetChunkFromBlock(x, y, z, false);
};

static auto GetWorldBlock = [&](int x, int y, int z) {
	static WorldManager& wManager = *((WorldManager*)Evt::GlobalEvt["wManager"]);
	return wManager.GetBlock(x, y, z);
};

WorldGenerator::WorldGenerator(Graphics& gfx)
	: gfx(gfx), chunkActionThreadData(threadCount)
{
	constexpr int maxScale = 4;
	constexpr int minScale = 2;
	worldScale = (rand() % (maxScale - minScale + 1)) + minScale;
	waterScale = (worldScale - 1) * 5;

	noise.SetNoiseType(FastNoise::NoiseType::ValueFractal);
	noise.SetFractalOctaves(1);
	noise.SetFrequency(0.01f);

	for (int i = 0; i < threadCount; i++) {
		threads.push_back(std::thread(&WorldGenerator::ThreadLoop, this));
	}
}

WorldGenerator::~WorldGenerator()
{
	running = false;
	for(int i = 0; i < threads.size(); i++){
		if (threads[i].joinable()) threads[i].join();
	}
}

void WorldGenerator::AddNewChunk(std::shared_ptr<Chunk> chunk)
{
	chunkActions.push({ ChunkAction::Actions::Generate, chunk });
}

void WorldGenerator::AddChunkForMeshing(std::shared_ptr<Chunk> chunk)
{
	chunkActions.push({ ChunkAction::Actions::Mesh, chunk });
}

bool WorldGenerator::BlockVisible(std::shared_ptr<Chunk> chunkPtr, int x, int y, int z, Block::BlockType type)
{
	if (chunkPtr == nullptr) return true;
	auto& chunk = *chunkPtr;
	if (y < 0) return true;
	if (x < Chunk::ChunkSize - 1 && y < Chunk::ChunkSize - 1 && z < Chunk::ChunkSize - 1
		&& x > 0 && y > 0 && z > 0)
	{
		auto block = chunk(x, y, z);
		return block.IsTransparent() && block.GetBlockType() != type;
	}
	else
	{
		auto chunk = GetChunkFromBlock(x, y, z);
		if (chunk == nullptr || chunk->blocks.empty()) return true;
		auto& block = chunk->operator()(x, y, z);
		return block.IsTransparent() && block.GetBlockType() != type;
	}
	return false;
}

void WorldGenerator::ThreadLoop()
{
	using namespace std::chrono_literals;
	ChunkGenerator chunkGen;
	while (running) {
		while (running && chunkActions.empty()) std::this_thread::sleep_for(10ms);
		if (!running) return;
		ChunkAction action = chunkActions.pop();
		if (action.chunk.use_count() > 1) {
			if (action.action == ChunkAction::Actions::Generate) {
				GenerateChunk(action.chunk, chunkGen);
			}
			else if (action.action == ChunkAction::Actions::Mesh) {
				GenerateMesh(action.chunk);
				//if (!GenerateMesh(action.chunk)) chunkActions.push(action);
			}
		}
	}
}

bool WorldGenerator::GenerateMesh(std::shared_ptr<Chunk> chunkPtr)
{
	chunkPtr->SafeToAccess = false;
	Chunk& chunk = *chunkPtr;
	/*Position pos = chunk.GetPosition();
	static auto IsChunkInvalid = [&](int x, int y, int z) {
		auto chunk = GetChunkFromBlock(x, y, z);
		if (chunk == nullptr || chunk->blocks.size() != Chunk::ChunkSize * Chunk::ChunkSize * Chunk::ChunkSize) return true;
		else return false;
	};
	if (IsChunkInvalid(pos.x + 24, pos.y, pos.z) ||
		IsChunkInvalid(pos.x - 8, pos.y, pos.z) ||
		IsChunkInvalid(pos.x, pos.y, pos.z + 24) ||
		IsChunkInvalid(pos.x, pos.y, pos.z - 8)) return false;*/

	if (std::all_of(std::execution::par_unseq, chunk.blocks.begin(), chunk.blocks.end(), [](Block& block) {return block.GetBlockType() == Block::BlockType::Air; })) return true;

	std::vector<Vertex> VertexBuffer;
	std::vector<uint16_t> IndexBuffer;

	std::vector<Vertex> AdditionalVertexBuffer;
	std::vector<uint16_t> AdditionalIndexBuffer;

	for (int y = Chunk::ChunkSize - 1; y >= 0; --y) {
		for (int x = Chunk::ChunkSize - 1; x >= 0; --x) {
			for (int z = Chunk::ChunkSize - 1; z >= 0 ; --z) {
				const Block& block = chunk(x, y, z);
				auto pos = chunk.GetPosition();
				pos.x += x;
				pos.y += y;
				pos.z += z;
				if (block.GetBlockType() == Block::BlockType::Air) continue;
				
				auto& TargetVertexBuffer = block.NeedsSeperateDrawCall() ? AdditionalVertexBuffer : VertexBuffer;
				auto& TargetIndexBuffer = block.NeedsSeperateDrawCall() ? AdditionalIndexBuffer : IndexBuffer;

				switch (block.GetMeshType()) {
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
	chunkPtr->SafeToAccess = true;
	
	return true;
}

void WorldGenerator::GenerateChunk(std::shared_ptr<Chunk> chunkPtr, ChunkGenerator& chunkGen)
{
	chunkPtr->blocks.resize(Chunk::ChunkSize * Chunk::ChunkSize * Chunk::ChunkSize);
	ChunkGenerator::chunkArray chunkArea;
	chunkArea[ChunkGenerator::ChunkPosition::Origin] = chunkPtr;

	Position pos = chunkPtr->GetPosition();
	chunkArea[ChunkGenerator::ChunkPosition::Top] = GetChunkFromBlock(pos.x, pos.y, pos.z + 1);
	chunkArea[ChunkGenerator::ChunkPosition::Bottom] = GetChunkFromBlock(pos.x, pos.y, pos.z - 1);
	chunkArea[ChunkGenerator::ChunkPosition::Left] = GetChunkFromBlock(pos.x - 1, pos.y, pos.z);
	chunkArea[ChunkGenerator::ChunkPosition::Right] = GetChunkFromBlock(pos.x + 1, pos.y, pos.z + 1);

	chunkGen.ProccessChunk(chunkArea);

	AddChunkForMeshing(chunkPtr);
}
