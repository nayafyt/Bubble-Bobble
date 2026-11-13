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

	// Size of the control hints drawn under the title.
	const float m_controls_size = 0.4f;

	// SGG cannot measure a string, so lay text out from the font's advance.
	void drawCenteredText(float y, float size, const std::string& text);

public:

	void drawSP();
	void initSP();

};