#pragma once
#include "XM_Structs.h"
#include "Block.h"
#include "RenderData.h"
#include "Graphics.h"

class BlockSelector {
	BlockSelector(const BlockSelector&) = delete;
	BlockSelector& operator=(const BlockSelector&) = delete;
public:
	BlockSelector(Graphics& gfx, Block::SelectorType type = Block::SelectorType::BLOCK);
	~BlockSelector() = default;

	void SetType(Block::SelectorType type, bool OverrideCheck = false);
	void SetTransforms(const Transforms& tf);

	void Render(Graphics& gfx);

private:
	RenderData renderData;
	Block::SelectorType type;
};
