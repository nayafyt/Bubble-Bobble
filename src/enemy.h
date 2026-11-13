#pragma once
#include "gameobject.h"
#include <sgg/graphics.h>
#include "box.h"

class Enemy :public Box, public GameObject {
	std::string name;
	float startingPosX;
	float pos_x;
	float pos_y;
	bool active = true;
	float startX, startY;
	float endX, endY;
	float moveSpeed;
	bool movingRight;
	bool movingDown;

	graphics::Brush m_brush_enemy;
	graphics::Brush m_enemy_brush_debug;


public:
	Enemy(std::string name) : GameObject(name) {}
	void update(float dt) override;
	void draw() override;
	void init() override;
	void init(float x, float y);

	float getStartingX() const { return startingPosX; }

	bool isActive() {
		return active;
	};

	void setInactive() {
		active = false;
	}
	Box getBox(int id) {
		return Box(pos_x, pos_y, 1.0f, 1.0f);
	}

protected:
	void debugDraw();
};