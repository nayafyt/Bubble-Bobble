#include "player.h"
#include "util.h"
#include <cmath>
#include <cstdio>

void Player::update(float dt)
{
	float delta_time = dt / 1000.0f;

	movePlayer(dt);

	// update offset for other game objects
	m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	GameObject::update(dt);

}

void Player::draw()
{
	if (m_sprites.empty())
		return;

	// fmod() returns a negative value once m_pos_x climbs past ~11.1 (and NaN
	// for an empty sprite list), so the raw result must not be used as an index
	// directly - doing so read past the end of m_sprites and crashed the game.
	const int frame_count = (int)m_sprites.size();
	int sprite = (int)fmod(100.0f - m_pos_x * 9.0f, (float)frame_count);
	sprite = ((sprite % frame_count) + frame_count) % frame_count;

	m_brush_player.texture = m_sprites[sprite];
	graphics::drawRect(m_state->getCanvasWidth() * 0.5f, m_state->getCanvasHeight() * 0.5f, 0.8f, 0.8f, m_brush_player);

	if (m_state->m_debugging)
		debugDraw();
}

void Player::init()
{
	// stage 1
	m_pos_x = 4.0f;
	m_pos_y = -1.0f;

	m_state->m_global_offset_x = m_state->getCanvasWidth() / 2.0f - m_pos_x;
	m_state->m_global_offset_y = m_state->getCanvasHeight() / 2.0f - m_pos_y;

	m_brush_player.fill_opacity = 1.0f;
	m_brush_player.outline_opacity = 0.0f;
	m_brush_player.texture = m_state->getFullAssetPath("Bub.png");

	m_sprites.push_back(m_state->getFullAssetPath("bub1.png"));
	m_sprites.push_back(m_state->getFullAssetPath("bub2.png"));
	m_sprites.push_back(m_state->getFullAssetPath("bub3.png"));
	m_sprites.push_back(m_state->getFullAssetPath("bub4.png"));

	// Adjust width for finer collision detection
	m_width = 0.5f;
}

void Player::debugDraw()
{
	graphics::Brush debug_brush;
	SETCOLOR(debug_brush.fill_color, 1, 0.3f, 0);
	SETCOLOR(debug_brush.outline_color, 1, 0.1f, 0);
	debug_brush.fill_opacity = 0.1f;
	debug_brush.outline_opacity = 1.0f;
	graphics::drawRect(m_state->getCanvasWidth() * 0.5f, m_state->getCanvasHeight() * 0.5f, m_width, m_height, debug_brush);

	char s[20];
	snprintf(s, sizeof(s), "(%5.2f, %5.2f)", m_pos_x, m_pos_y);
	SETCOLOR(debug_brush.fill_color, 1, 0, 0);
	debug_brush.fill_opacity = 1.0f;
	graphics::drawText(m_state->getCanvasWidth() * 0.5f - 0.4f, m_state->getCanvasHeight() * 0.5f - 0.6f, 0.15f, s, debug_brush);
}

void Player::movePlayer(float dt)
{
	float delta_time = dt / 1000.0f;

	// Stage 2 code: Acceleration-based velocity
	// Steering accepts either A/D or the left/right arrow keys.
	const bool move_left = graphics::getKeyState(graphics::SCANCODE_A) ||
		graphics::getKeyState(graphics::SCANCODE_LEFT);
	const bool move_right = graphics::getKeyState(graphics::SCANCODE_D) ||
		graphics::getKeyState(graphics::SCANCODE_RIGHT);

	float move = 0.0f;
	if (move_left)
		move -= 1.0f;
	if (move_right)
		move = 1.0f;

	m_vx = std::min<float>(m_max_velocity, m_vx + delta_time * move * m_accel_horizontal);
	m_vx = std::max<float>(-m_max_velocity, m_vx);

	// friction
	m_vx -= 0.2f * m_vx / (0.1f + fabs(m_vx));

	// apply static friction threshold
	if (fabs(m_vx) < 0.01f)
		m_vx = 0.0f;

	// adjust horizontal position
	m_pos_x += m_vx * delta_time;

	// jump only when not in flight:
	if (m_vy == 0.0f) {
		// W or the up arrow key
		const bool jump = graphics::getKeyState(graphics::SCANCODE_W) ||
			graphics::getKeyState(graphics::SCANCODE_UP);
		m_vy -= (jump ? m_accel_vertical : 0.0f) * 0.02f;// not delta_time!! Burst 
	}

	// add gravity
	m_vy += delta_time * m_gravity;

	// adjust vertical position
	m_pos_y += m_vy * delta_time;


}
