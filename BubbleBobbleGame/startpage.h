#pragma once
#include "gameobject.h"
#include <sgg/graphics.h>
#include <string>

class StartPage : public GameObject
{
	graphics::Brush m_brush_background;

	const float m_block_size = 1.0f;
	graphics::Brush m_block_brush;
	graphics::Brush m_block_brush_debug;
	graphics::Brush m_brush_text;
	graphics::Brush m_brush_title;

	// "PRESS ENTER TO START", and the two tiers of the controls table
	graphics::Brush m_brush_prompt;
	graphics::Brush m_brush_controls_head;
	graphics::Brush m_brush_controls;

	// SGG cannot measure a rendered string, so lay text out from the font's
	// advance width instead.
	float textWidth(const std::string& text, float size) const;
	void drawCenteredText(float y, float size, const std::string& text,
	                      graphics::Brush& brush);

public:

	void drawSP();
	void initSP();

};
