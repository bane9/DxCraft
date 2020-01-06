#pragma once
#include "robin_hood.h"
#include <DirectXMath.h>
#include <math.h>

struct Position {
	Position(int x, int y, int z) : x(x), y(y), z(z) {}
	Position() = default;
	Position(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); }
	Position& operator=(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); return *this; }
	bool operator==(const Position& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
	Position operator+(const Position& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	int x = 0, y = 0, z = 0;
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