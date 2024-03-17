#pragma once

#include "gameobject.h"
#include <sgg/graphics.h>
#include "box.h"

class Player : public Box, public GameObject
{
	std::vector<std::string> m_sprites;

	graphics::Brush m_brush_player;

	const float m_accel_horizontal = 200.0f;
	const float m_accel_vertical = 300.1f;
	const float m_max_velocity = 5.0f;
	const float m_gravity = 10.0f;
	int score = 0;
	int hearts = 5;

public:
	float m_vx = 0.0f;
	float m_vy = 0.0f;

public:
	void update(float dt) override;
	void draw() override;
	void init() override;

	void addScore(int i) { score += i; }
	void reduceScore(int i) { score -= i; }
	int getScore() { return score; }
	void resetScore() { score = 0; }

	void loseHeart() { hearts -= 1; }
	int getHearts() { return hearts; }


	Player(std::string name) : GameObject(name) {}

protected:
	void debugDraw();

	// dynamic motion control
	void movePlayer(float dt);
};
