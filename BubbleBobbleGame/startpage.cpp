#include "startpage.h"

void StartPage::drawSP()
{
	float w = m_state->getCanvasWidth();
	float h = m_state->getCanvasHeight();


	float offset_x = m_state->m_global_offset_x / 2.0f + w / 2.0f;
	float offset_y = m_state->m_global_offset_y / 2.0f + h / 2.0f;

	graphics::drawRect(offset_x, offset_y - 2, w / 2.0f, h / 2.0f, m_brush_background);
	graphics::drawRect(w / 4 + 5, h / 2 + 7.5, w / 2.0f, h / 10.0f, m_brush_title);

	graphics::drawText(w / 4 + 1, h / 2 + 5, 0.5, "Press ENTER to Start", m_brush_text);

	// Spell out the controls, in the empty strip above the logo - otherwise
	// there is nowhere in the game that tells you which keys do anything.
	drawCenteredText(1.5f, m_controls_size, "MOVE:  A / D  OR  LEFT / RIGHT");
	drawCenteredText(2.6f, m_controls_size, "JUMP:  W  OR  UP");

}

void StartPage::drawCenteredText(float y, float size, const std::string& text)
{
	// The title screen runs in PressStart2P, which is monospaced; an advance
	// of about 0.76 * size matches the "Press ENTER to Start" line above.
	const float width = text.length() * size * 0.76f;
	graphics::drawText((m_state->getCanvasWidth() - width) / 2.0f, y, size, text, m_brush_text);
}

void StartPage::initSP()
{

	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.texture = m_state->getFullAssetPath("start1.png");

	m_brush_text.fill_color[0] = 1.0f;
	m_brush_text.fill_color[1] = 1.0f;
	m_brush_text.fill_color[2] = 1.0f;
	graphics::playMusic(m_state->getFullAssetPath("intro.wav"), 0.11f, true);

	m_brush_title.outline_opacity = 0.0f;
	m_brush_title.texture = m_state->getFullAssetPath("title.png");

}

