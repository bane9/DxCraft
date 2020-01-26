#include "WorldGenerator.h"
#include "EventManager.h"
#include "WorldManager.h"
#include "BillBoard.h"
#include <algorithm>

static auto GetChunkFromBlock = [&](int x, int y, int z) {
	static WorldManager& wManager = *((WorldManager*)Evt::GlobalEvt["wManager"]);
	return wManager.GetChunkFromBlock(x, y, z, false);
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
	noise.SetSeed(time(0));

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
	while (running) {
		while (running && chunkActions.empty()) std::this_thread::sleep_for(10ms);
		if (!running) return;
		ChunkAction action = chunkActions.pop();
		if (action.chunk.use_count() > 1) {
			if (action.action == ChunkAction::Actions::Generate) {
				GenerateChunk(action.chunk);
			}
			else if (action.action == ChunkAction::Actions::Mesh) {
				GenerateMesh(action.chunk);
			}
		}
	}
}

void WorldGenerator::GenerateMesh(std::shared_ptr<Chunk> chunkPtr)
{
	chunkPtr->SafeToAccess = false;
	Chunk& chunk = *chunkPtr;
	std::vector<Vertex> VertexBuffer;
	std::vector<uint16_t> IndexBuffer;

	std::vector<Vertex> AdditionalVertexBuffer;
	std::vector<uint16_t> AdditionalIndexBuffer;

	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int y = 0; y < Chunk::ChunkSize; y++) {
			for (int z = 0; z < Chunk::ChunkSize; z++) {
				const Block& block = chunk(x, y, z);
				if (block.GetBlockType() == Block::BlockType::Air) continue;
				auto pos = chunk.GetPosition();
				pos.x += x;
				pos.y += y;
				pos.z += z;

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
}

void WorldGenerator::GenerateChunk(std::shared_ptr<Chunk> chunkPtr)
{
	chunkPtr->blocks.resize(Chunk::ChunkSize * Chunk::ChunkSize * Chunk::ChunkSize);
	Position pos = chunkPtr->GetPosition();
	for (int x = 0; x < Chunk::ChunkSize; x++) {
		for (int z = 0; z < Chunk::ChunkSize; z++) {
			auto GetBlock = [chunkPtr](int x, int y, int z) {
				Position normalized = chunkPtr->Normalize(x, y, z);
				return &chunkPtr->operator()(normalized.x, normalized.y, normalized.z);
			};
			constexpr float prescale = 50.0f;
			float height = prescale + std::clamp(
				(noise.GetNoise(pos.x + x, pos.z + z) / 2.0f + 0.5f) * (Chunk::ChunkSize - 1) * worldScale,
				0.0f,
				205.0f);
			for (int y = 0; y < std::clamp((int)height - chunkPtr->y, 0, Chunk::ChunkSize); y++) {
				auto block = GetBlock(x, y, z);
				const float scale = height / (worldScale * 16);
				if (chunkPtr->y + y == 0) block->SetBlockType(Block::BlockType::Bedrock);
				else if (height >= prescale + waterScale) {
					if (height > scale * 0.98f)
						block->SetBlockType(Block::BlockType::Grass);
					else if (height > scale * 0.75f && height + y < scale * 0.98f)
						block->SetBlockType(Block::BlockType::Dirt);
					else
						block->SetBlockType(Block::BlockType::Stone);
				}
				else {
					if (height > scale * 0.95f)
						block->SetBlockType(Block::BlockType::Dirt);
					else
						block->SetBlockType(Block::BlockType::Stone);
				}
			}
		}
	}

	AddChunkForMeshing(chunkPtr);
}
