#pragma once
#include "robin_hood.h"
#include <DirectXMath.h>
#include <math.h>

struct Position {
	Position(int x, int y, int z) : x(x), y(y), z(z) {}
	Position() = default;
	Position(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); }
	Position& operator=(const DirectX::XMFLOAT3& pos) { XMFLOAT3ToPos(pos); return *this; }
	bool operator==(const Position& other) const noexcept{
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator!=(const Position& other) const noexcept{
		return !operator==(other);
	}
	Position operator+(const Position& other) const noexcept{
		return { x + other.x, y + other.y, z + other.z };
	}
	Position operator-(const Position& other) const noexcept {
		return { x - other.x, y - other.y, z - other.z };
	}
	bool operator<(const Position& rhs) const noexcept{
		if (x != rhs.x) return x < rhs.x;
		else if (y != rhs.y) return y < rhs.y;
		else return z < rhs.z;
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
		return (73856093 * position.x ^ 19349663 * position.y ^ 83492791 * position.z) & std::numeric_limits<int>::max();
	}
};

struct Position2 {
	Position2(int x, int z) : x(x), z(z) {}
	Position2() = default;
	bool operator==(const Position2& other) const noexcept {
		return x == other.x && z == other.z;
	}
	bool operator!=(const Position2& other) const noexcept {
		return !operator==(other);
	}
	Position2 operator+(const Position2& other) const noexcept {
		return { x + other.x, z + other.z };
	}
	bool operator<(const Position2& rhs) const noexcept {
		if (x != rhs.x) return x < rhs.x;
		else return z < rhs.z;
	}
	int x = 0, z = 0;
};
