#include "block.h"

void Block::buildBlock(float x, float y, float width, float height, const std::string& name, float startTime) {
	m_blocks.push_back(Box(x, y, width, height));
	m_block_names.push_back(name);
	m_blockStartTime.push_back(startTime);

}

void Block::erase(int i)
{
	m_blocks.erase(m_blocks.begin() + i);
	m_block_names.erase(m_block_names.begin() + i);
	m_blockStartTime.erase(m_blockStartTime.begin() + i);
}
