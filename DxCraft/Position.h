#pragma once
#include "robin_hood.h"

struct Position {
	Position(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
	Position() = delete;
	bool operator==(const Position& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
	int x, y, z;
};

struct PositionHash {
	std::size_t operator()(const Position& position) const {
		robin_hood::hash<int> hash;
		return hash(position.x) ^ hash(position.y) ^ hash(position.z);
	}
};