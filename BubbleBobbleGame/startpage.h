#pragma once
#include "gameobject.h"
#include <sgg/graphics.h>

class StartPage : public GameObject
{
	graphics::Brush m_brush_background;

	const float m_block_size = 1.0f;
	graphics::Brush m_block_brush;
	graphics::Brush m_block_brush_debug;
	graphics::Brush m_brush_text;
	graphics::Brush m_brush_title;

public:

	void drawSP();
	void initSP();

};