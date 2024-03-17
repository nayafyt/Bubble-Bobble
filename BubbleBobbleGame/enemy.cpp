#include "enemy.h"
#include "sgg/graphics.h"
#include "util.h"
#include <iostream>

void Enemy::update(float dt) {
	float delta_time = dt / 1000.0f;
	if (startingPosX == 0) {
		if (movingRight) {
			pos_x += moveSpeed * delta_time;
			if (pos_x >= endX) {
				pos_x = endX;
				pos_y = 4.0f;
				startX = 1.0f;
				endX = 7.0f;
				movingRight = false;
			}
		}
		else {
			pos_x -= moveSpeed * delta_time;
			if (pos_x <= startX) {
				pos_x = startX;
				movingRight = true;
			}
		}
	}

	if (startingPosX == 3 || startingPosX == -3 || startingPosX == -1) {
		if (movingRight) {
			pos_x += moveSpeed * delta_time;
			if (pos_x >= endX) {
				pos_x = endX;
				startX = -2.0f;
				endX = 8.0f;
				movingRight = false;
			}
		}
		else {
			pos_x -= moveSpeed * delta_time;
			if (pos_x <= startX) {
				pos_x = startX;
				movingRight = true;
			}
		}

	}
	if (startingPosX == 8 || startingPosX == -2) {
		if (movingDown) {
			pos_y += moveSpeed * delta_time;
			if (pos_y >= endY) {
				pos_y = endY;
				startY = -8.0f;
				endY = 5.0f;
				movingDown = false;
			}
		}
		else {
			pos_y -= moveSpeed * delta_time;
			if (pos_y <= startY) {
				pos_y = startY;
				movingDown = true;
			}
		}

	}
	if (startingPosX == 4) {
		if (movingRight) {
			pos_x += moveSpeed * delta_time;
			if (pos_x >= endX) {
				pos_x = endX;
				movingRight = false;
			}
		}
		else {
			pos_x -= moveSpeed * delta_time;
			if (pos_x <= startX) {
				startX = -2.0f;
				endX = 2.0f;
				pos_x = startX;
				movingRight = true;
			}
		}
	}
	if (startingPosX == 5) {
		if (movingDown) {
			pos_y += moveSpeed * delta_time;
			if (pos_y >= endY) {
				pos_y = endY;
				startY = -8.0f;
				endY = -3.0f;
				movingDown = false;
			}
		}
		else {
			pos_y -= moveSpeed * delta_time;
			if (pos_y <= startY) {
				pos_y = startY;
				movingDown = true;
			}
		}

	}
}

void Enemy::draw() {
	m_brush_enemy.texture = m_state->getFullAssetPath("enemy.png");
	m_brush_enemy.outline_opacity = 0.0f;

	float x = pos_x + m_state->m_global_offset_x;
	float y = pos_y + m_state->m_global_offset_y;
	graphics::drawRect(x, y, 0.7f, 0.7f, m_brush_enemy);

	if (m_state->m_debugging)
		debugDraw();
}

void Enemy::init() {
	pos_x = 0.0f;
	startingPosX = pos_x;
	pos_y = 1.0f;
	startX = pos_x;
	endX = startX + 2.0f;
	moveSpeed = 1.0f;
	movingRight = true;
	movingDown = false;
}

void Enemy::init(float x, float y) {
	pos_x = x;
	startingPosX = pos_x;
	pos_y = y;
	startX = pos_x;
	endX = startX + 2.0f;
	moveSpeed = 1.0f;
	movingDown = false;
	movingRight = true;
	if (pos_x == 4) {
		movingRight = false;
	}
}
void Enemy::debugDraw()
{
	graphics::Brush enemy_debug_brush;
	SETCOLOR(enemy_debug_brush.fill_color, 1, 0.3f, 0);
	SETCOLOR(enemy_debug_brush.outline_color, 1, 0.1f, 0);
	enemy_debug_brush.fill_opacity = 0.1f;
	enemy_debug_brush.outline_opacity = 1.0f;
	graphics::drawRect(pos_x + m_state->m_global_offset_x, pos_y + m_state->m_global_offset_y, 0.7f, 0.7f, enemy_debug_brush);
	char s[20];
	sprintf_s(s, "(%5.2f, %5.2f)", pos_x, pos_y);
	graphics::drawText(pos_x + m_state->m_global_offset_x - 0.4f, pos_y + m_state->m_global_offset_y - 0.6f, 0.15f, s, enemy_debug_brush);
}

