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

void WorldGenerator::ThreadLoop()
{
	using namespace std::chrono_literals;
	ChunkGenerator chunkGen;
	while (running) {
		while (running && chunkActions.empty()) std::this_thread::sleep_for(10ms);
		if (!running) return;
		ChunkAction action = chunkActions.popOrDefault();
		if (action.chunk != nullptr && action.chunk.use_count() > 1) {		
			if (action.action == ChunkAction::Actions::Generate) {
				GenerateChunk(action.chunk, chunkGen);
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
	chunkPtr->OcludedOnNull = false;
	Chunk& chunk = *chunkPtr;
		
	std::array<std::shared_ptr<Chunk>, 7> chunks;

	chunks[0] = chunkPtr;

	chunks[1] = GetChunkFromBlock(chunkPtr->x + Chunk::ChunkSize, chunkPtr->y, chunkPtr->z);
	if (chunks[1] != nullptr && !chunks[1]->HasGenerated) chunks[1] = nullptr;

	chunks[2] = GetChunkFromBlock(chunkPtr->x - 1, chunkPtr->y, chunkPtr->z);
	if (chunks[2] != nullptr && !chunks[2]->HasGenerated) chunks[2] = nullptr;

	chunks[3] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y + Chunk::ChunkSize, chunkPtr->z);
	if (chunks[3] != nullptr && !chunks[3]->HasGenerated) chunks[3] = nullptr;

	chunks[4] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y - 1, chunkPtr->z);
	if (chunks[4] != nullptr && !chunks[4]->HasGenerated) chunks[4] = nullptr;

	chunks[5] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y, chunkPtr->z + Chunk::ChunkSize);
	if (chunks[5] != nullptr && !chunks[5]->HasGenerated) chunks[5] = nullptr;

	chunks[6] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y, chunkPtr->z - 1);
	if (chunks[6] != nullptr && !chunks[6]->HasGenerated) chunks[6] = nullptr;

	enum class Visibility {
		Visible,
		NotVisible,
		Null
	};

	auto BlockVisible = [&chunks](int x, int y, int z, 
		Block::BlockType type = Block::BlockType::None) {
		if (y < 0) return Visibility::Visible;

		Position chunkPosition(
			x - FixedMod(x, Chunk::ChunkSize),
			y - FixedMod(y, Chunk::ChunkSize),
			z - FixedMod(z, Chunk::ChunkSize)
		);

		for (auto& chunk : chunks) {
			if (chunk != nullptr && chunkPosition == chunk->GetPosition()) {
				Position normalized = chunk->Normalize(x, y, z);
				auto& block = chunk->operator()(normalized.x, normalized.y, normalized.z);
				return (block.IsTransparent() && block.GetBlockType() != type) ? Visibility::Visible : Visibility::NotVisible;
			}
			else if (chunk == nullptr) {
				Position offset = chunks[0]->GetPosition() - chunkPosition;
				if (offset.x == Chunk::ChunkSize && chunks[2] == nullptr) return Visibility::Null;
				else if (offset.x == -Chunk::ChunkSize && chunks[1] == nullptr) return Visibility::Null;
				else if (offset.y == Chunk::ChunkSize && chunks[4] == nullptr) return Visibility::Null;
				else if (offset.y == -Chunk::ChunkSize && chunks[3] == nullptr) return Visibility::Null;
				else if (offset.z == Chunk::ChunkSize && chunks[6] == nullptr) return Visibility::Null;
				else if (offset.z == -Chunk::ChunkSize && chunks[5] == nullptr) return Visibility::Null;
			}
		}
		
	};

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
					auto result = BlockVisible(pos.x + 1, pos.y, pos.z);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x - 1, pos.y, pos.z);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y + 1, pos.z);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y - 1, pos.z);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y, pos.z + 1);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y, pos.z - 1);
					if (result == Visibility::Visible) {
						AppendMesh(Faces::NearSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[1],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;
				}
				else {
					auto result = BlockVisible(pos.x + 1, pos.y, pos.z, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::RightSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[3],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x - 1, pos.y, pos.z, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::LeftSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[2],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y + 1, pos.z, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::TopSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[5],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y - 1, pos.z, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::BottomSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[4],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y, pos.z + 1, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::FarSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[0],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;

					result = BlockVisible(pos.x, pos.y, pos.z - 1, block.GetBlockType());
					if (result == Visibility::Visible) {
						AppendMesh(Faces::NearSide, TargetVertexBuffer, TargetIndexBuffer, block.GetTexCoords()[1],
							x, y, z);
					}
					else if (result == Visibility::Null) chunk.OcludedOnNull = true;
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
	chunkPtr->HasGenerated = true;

	std::array<std::shared_ptr<Chunk>, 6> chunks;

	chunks[0] = GetChunkFromBlock(chunkPtr->x + Chunk::ChunkSize, chunkPtr->y, chunkPtr->z);
	if (chunks[0] != nullptr && !chunks[0]->SafeToAccess && !chunks[0]->OcludedOnNull) chunks[0] = nullptr;

	chunks[1] = GetChunkFromBlock(chunkPtr->x - 1, chunkPtr->y, chunkPtr->z);
	if (chunks[1] != nullptr && !chunks[1]->SafeToAccess && !chunks[1]->OcludedOnNull) chunks[1] = nullptr;

	chunks[2] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y + Chunk::ChunkSize, chunkPtr->z);
	if (chunks[2] != nullptr && !chunks[2]->SafeToAccess && !chunks[2]->OcludedOnNull) chunks[2] = nullptr;

	chunks[3] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y - 1, chunkPtr->z);
	if (chunks[3] != nullptr && !chunks[3]->SafeToAccess && !chunks[3]->OcludedOnNull) chunks[3] = nullptr;

	chunks[4] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y, chunkPtr->z + Chunk::ChunkSize);
	if (chunks[4] != nullptr && !chunks[4]->SafeToAccess && !chunks[4]->OcludedOnNull) chunks[4] = nullptr;

	chunks[5] = GetChunkFromBlock(chunkPtr->x, chunkPtr->y, chunkPtr->z - 1);
	if (chunks[5] != nullptr && !chunks[5]->SafeToAccess && !chunks[5]->OcludedOnNull) chunks[5] = nullptr;

	for (auto& chunk : chunks) {
		if(chunk != nullptr) AddChunkForMeshing(chunk);
	}

	AddChunkForMeshing(chunkPtr);
}
