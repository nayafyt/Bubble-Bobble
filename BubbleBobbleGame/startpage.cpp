#include "startpage.h"
#include <algorithm>

// The title screen is laid out in canvas units (the canvas is 20 x 20).
// SGG cannot measure a rendered string, so text is positioned from the font's
// advance widths. PressStart2P is monospaced, but its space is far narrower
// than a glyph, so the two are measured separately - and columns are placed
// explicitly rather than padded with spaces, which would not line up.
static const float GLYPH_ADVANCE = 0.88f;
static const float SPACE_ADVANCE = 0.18f;

float StartPage::textWidth(const std::string& text, float size) const
{
	const size_t spaces = std::count(text.begin(), text.end(), ' ');
	const size_t glyphs = text.length() - spaces;
	return (glyphs * GLYPH_ADVANCE + spaces * SPACE_ADVANCE) * size;
}

void StartPage::drawCenteredText(float y, float size, const std::string& text,
                                 graphics::Brush& brush)
{
	const float x = (m_state->getCanvasWidth() - textWidth(text, size)) / 2.0f;
	graphics::drawText(x, y, size, text, brush);
}

void StartPage::drawSP()
{
	float w = m_state->getCanvasWidth();

	// Logo. start_logo.png is a 512x512 (power-of-two, so SGG leaves it alone)
	// copy of the artwork, nearest-neighbour upscaled so the pixels stay crisp.
	// It is padded to a square, so drawing it square keeps its aspect ratio.
	graphics::drawRect(w / 2.0f, 5.4f, 12.5f, 12.5f, m_brush_background);

	// A single space is barely visible in this font, so pad the gaps out.
	drawCenteredText(11.1f, 0.62f, "PRESS   ENTER   TO   START", m_brush_prompt);

	// Controls table. Each column is drawn at its own x so they actually align.
	const float size = 0.36f;
	const float col_label = 4.95f;
	const float col_keys = 8.35f;
	const float col_arrows = 11.75f;

	graphics::drawText(col_keys, 13.1f, size, "KEYS", m_brush_controls_head);
	graphics::drawText(col_arrows, 13.1f, size, "ARROWS", m_brush_controls_head);

	graphics::drawText(col_label, 14.3f, size, "MOVE", m_brush_controls);
	graphics::drawText(col_keys, 14.3f, size, "A / D", m_brush_controls);
	graphics::drawText(col_arrows, 14.3f, size, "LEFT / RIGHT", m_brush_controls);

	graphics::drawText(col_label, 15.4f, size, "JUMP", m_brush_controls);
	graphics::drawText(col_keys, 15.4f, size, "W", m_brush_controls);
	graphics::drawText(col_arrows, 15.4f, size, "UP", m_brush_controls);

	// Taito / Nintendo credits strip. title.png is 172x38, so draw it at that
	// aspect instead of stretching it, and keep it clear of the bottom edge.
	graphics::drawRect(w / 2.0f, 18.0f, 8.0f, 8.0f * 38.0f / 172.0f, m_brush_title);
}

void StartPage::initSP()
{

	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.texture = m_state->getFullAssetPath("start_logo.png");

	m_brush_prompt.fill_color[0] = 1.0f;
	m_brush_prompt.fill_color[1] = 1.0f;
	m_brush_prompt.fill_color[2] = 1.0f;
	m_brush_prompt.fill_opacity = 1.0f;
	m_brush_prompt.outline_opacity = 0.0f;

	// warm yellow, picked out of the logo
	m_brush_controls_head.fill_color[0] = 1.0f;
	m_brush_controls_head.fill_color[1] = 0.72f;
	m_brush_controls_head.fill_color[2] = 0.25f;
	m_brush_controls_head.outline_opacity = 0.0f;

	m_brush_controls.fill_color[0] = 0.84f;
	m_brush_controls.fill_color[1] = 0.84f;
	m_brush_controls.fill_color[2] = 0.90f;
	m_brush_controls.outline_opacity = 0.0f;

	graphics::playMusic(m_state->getFullAssetPath("intro.wav"), 0.11f, true);

	m_brush_title.outline_opacity = 0.0f;
	m_brush_title.texture = m_state->getFullAssetPath("title.png");

}
