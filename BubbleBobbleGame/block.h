#pragma once
#include <vector>
#include <list>
#include <string>
#include "box.h"

class Block {
	std::vector<Box> m_blocks;
	std::vector<std::string> m_block_names;
	std::vector<float> m_blockStartTime;
public:
	void buildBlock(float x, float y, float width = 1.0f, float height = 1.0f, const std::string& name = "tiles.png", float startTime = 0.0f);
	Box& getBox(int i) { return m_blocks[i]; }
	std::string& getName(int i) { return m_block_names[i]; }
	std::vector<Box> getBlocks() { return m_blocks; };
	std::vector<std::string> getBlockNames() { return m_block_names; };
	float getBlockStartTime(int i) { return m_blockStartTime[i]; };
	//to remove the coins blocks, destroy specific box 
	//erase is a built-in function of a vector
	void erase(int i);
};