#pragma once
#include "gameobject.h"
#include <sgg/graphics.h>

class EndPage : public GameObject {
	graphics::Brush m_brush_background;
	graphics::Brush m_brush_text;
	graphics::Brush m_brush_title;
public:
	void drawEP();
	void initEP();
};