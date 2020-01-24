#pragma once
#include <memory>
#include <list>
#include <vector>
#include <thread>
#include "Block.h"
#include "Graphics.h"
#include "BasicChunk.h"
#include "FastNoise.h"
#include <mutex>

class WorldGenerator
{
private:
	struct ChunkAction {
		enum class Actions {
			Invalid, 
			Generate,
			Mesh
		};
		Actions action = Actions::Invalid;
		std::shared_ptr<Chunk> chunk;
	};

	struct ThreadLocks {
		std::mutex mutex;
		std::unique_lock<std::mutex> lock;
		std::condition_variable cv;
		ThreadLocks() : lock(mutex) {}
	};

	struct Threads {
		std::thread thread;
		int index;
		bool running = false;
		Threads(WorldGenerator* wGenInstance, int index) : index(index), thread(&WorldGenerator::ThreadLoop, wGenInstance, index) {};
	};

public:
	WorldGenerator(Graphics& gfx);
	~WorldGenerator();
	void Loop();
	void AddNewChunk(std::shared_ptr<Chunk> chunk);
	void AddChunkForMeshing(std::shared_ptr<Chunk> chunk);
	
private:
	void ThreadLoop(int index);
	void GenerateMesh(std::shared_ptr<Chunk> chunkPtr);
	void GenerateChunk(std::shared_ptr<Chunk> chunkPtr);
	template<typename Container, typename UVs>
	void AppendMesh(const Container& mesh,
		std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
		const UVs& textures, float offsetX, float offsetY, float offsetZ);
	bool BlockVisible(std::shared_ptr<Chunk>, int x, int y, int z, Block::BlockType type = Block::BlockType::None);

	int coreCount = std::thread::hardware_concurrency();
	std::vector<Threads> threads;
	std::vector<ThreadLocks> threadLocks;
	std::vector<ChunkAction> chunkActionThreadData;

	bool running = true;
	Graphics& gfx;
	FastNoise noise;
	int worldScale, waterScale;
	std::list<ChunkAction> chunkActions;
};

template <class>
static constexpr bool is_array_of_array_v = false;

template <class T, std::size_t N, std::size_t M>
static constexpr bool is_array_of_array_v<std::array<std::array<T, N>, M>> = true;

template<typename Container, typename UVs>
inline void WorldGenerator::AppendMesh(const Container& mesh,
	std::vector<Vertex>& vertexBuffer, std::vector<uint16_t>& indexBuffer,
	const UVs& textures, float offsetX, float offsetY, float offsetZ)
{
	if constexpr (is_array_of_array_v<UVs>) {
		for (int i = 0; i < textures.size(); i++) {
			std::transform(mesh.first.begin(), mesh.first.end(), std::back_inserter(vertexBuffer),
				[offsetX, offsetY, offsetZ, &textures, i](Vertex vertex) {
					vertex.pos.x += offsetX;
					vertex.pos.y += offsetY;
					vertex.pos.z += offsetZ;
					float startU, endU, startV, endV;
					startU = textures[i][0] / 16.0f;
					endU = (textures[i][0] + 1.0f) / 16.0f;
					startV = textures[i][1] / 16.0f;
					endV = (textures[i][1] + 1.0f) / 16.0f;
					vertex.tc.x = vertex.tc.x == 1 ? startU : endU;
					vertex.tc.y = vertex.tc.y == 1 ? startV : endV;
					return vertex;
				});
		}
	}
	else {
		std::transform(mesh.first.begin(), mesh.first.end(), std::back_inserter(vertexBuffer),
			[offsetX, offsetY, offsetZ, &textures](Vertex vertex) {
				vertex.pos.x += offsetX;
				vertex.pos.y += offsetY;
				vertex.pos.z += offsetZ;
				float startU, endU, startV, endV;
				startU = textures[0] / 16.0f;
				endU = (textures[0] + 1.0f) / 16.0f;
				startV = textures[1] / 16.0f;
				endV = (textures[1] + 1.0f) / 16.0f;
				vertex.tc.x = vertex.tc.x == 1 ? startU : endU;
				vertex.tc.y = vertex.tc.y == 1 ? startV : endV;
				return vertex;
			});
	}
	const int offset = vertexBuffer.size() > 0 ? (vertexBuffer.size() / mesh.first.size() - 1) * mesh.first.size() : 0;
	std::transform(mesh.second.begin(), mesh.second.end(), std::back_inserter(indexBuffer),
		[offset](int a) {return offset + a; });
}


