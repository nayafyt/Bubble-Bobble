#pragma once

#include "gameobject.h"
#include <sgg/graphics.h>
#include "player.h"
#include "enemy.h"
#include "block.h"
#include "notifymessage.h"


class Level : public GameObject
{
	graphics::Brush m_brush_background;

	std::vector<GameObject*> m_static_objects;
	std::list<GameObject*> m_dynamic_objects;

	std::vector<Enemy*> m_enemies;


	// add some collidable blocks
	//std::vector<Box> m_blocks;
	//std::vector<std::string> m_block_names;

	const float m_block_size = 1.0f;
	graphics::Brush m_block_brush;
	graphics::Brush m_block_brush_debug;
	graphics::Brush m_score_brush;

	bool m_gameEnded = false;
	//we are keeping the message of the notification 
	std::vector<NotifyMessage> m_activeMessages;

	float m_center_x = 5.0f;
	float m_center_y = 5.0f;

	// dedicated method to draw a block
	void drawBlock(int i, std::string type = "B");

	//initializing with 2 sec
	void updateMessage(float dt, float duration = 2.0f);



	//built a Block Object that is used by all classes that include "level.h" (public), it doesn't contain dynamic objects so there
	//is no need to delete it because it will be deleted when the function is not used.
	Block block;
	//Building anothe block object for the messages
	Block notifications;


	// detect collisions
	void checkCollisions();

public:
	void update(float dt) override;
	void draw() override;
	void init() override;


	Level(const std::string& name = "Level0");
	~Level() override;
};