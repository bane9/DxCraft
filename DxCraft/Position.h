#pragma once
#include "robin_hood.h"
#include <DirectXMath.h>
#include <math.h>

struct Position {
	Position(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
	Position() = delete;
	Position(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); }
	Position& operator=(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); }
	bool operator==(const Position& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
	Position operator+(const Position& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	int x, y, z;
private:
	void XMFLOAT3ToPos(const DirectX::XMFLOAT3& pos) {
		x = static_cast<int>(round(pos.x));
		y = static_cast<int>(round(pos.y));
		z = static_cast<int>(round(pos.z));
	}
};

struct PositionHash {
	std::size_t operator()(const Position& position) const {
		static robin_hood::hash<int> hash;
		return hash(position.x) ^ hash(position.y) ^ hash(position.z);
	}
};