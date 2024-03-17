#include "level.h"
#include <sgg/graphics.h>
#include "util.h"
#include <iostream>

void Level::drawBlock(int i, std::string type)
{
	Box box;
	std::string name;
	if (type == "B") {
		box = block.getBox(i);
		name = block.getName(i);
	}
	else {
		box = notifications.getBox(i);
		name = notifications.getName(i);
	}

	float x = box.m_pos_x + m_state->m_global_offset_x;
	float y = box.m_pos_y + m_state->m_global_offset_y;

	m_block_brush.texture = m_state->getFullAssetPath(name);

	graphics::drawRect(x, y, m_block_size * 1.0f, m_block_size * 1.0f, m_block_brush);

	if (m_state->m_debugging)
		graphics::drawRect(x, y, m_block_size, m_block_size, m_block_brush_debug);

}

void Level::updateMessage(float dt, float duration) {
	auto currentTime = graphics::getGlobalTime();
	int k = 0;
	for (int i = 0; i < notifications.getBlocks().size(); i++)
	{
		if (currentTime - notifications.getBlockStartTime(i) > duration * 1000) { // Check if duration secs have passed
			//Delete the block associated with the message
			notifications.erase(i);
			break;
		}

	}

}


void Level::checkCollisions()
{
	/*
	for (auto& box : m_blocks)
	{
		if (m_state->getPlayer()->intersect(box))
			printf("*");
	}
	*/

	for (auto& block : block.getBlocks())
	{
		float offset = 0.0f;
		if (offset = m_state->getPlayer()->intersectDown(block))
		{
			m_state->getPlayer()->m_pos_y += offset;

			// add sound event
			if (m_state->getPlayer()->m_vy > 1.0f)
				graphics::playSound(m_state->getFullAssetPath("jumpingNoise.wav"), 1.0f);

			m_state->getPlayer()->m_vy = 0.0f;

			break;
		}
	}

	for (auto& block : block.getBlocks())
	{
		float offset = 0.0f;
		if (offset = m_state->getPlayer()->intersectSideways(block))
		{
			m_state->getPlayer()->m_pos_x += offset;

			m_state->getPlayer()->m_vx = 0.0f;
			break;
		}

	}

	//Differentiate when you find block of name=gemGold.png

	for (int i = 0; i < block.getBlocks().size(); i++)
	{
		float offset = 0.0f;
		// Check for collisions with the player and the specific block/the coin
		if (offset = m_state->getPlayer()->intersect(block.getBox(i)))
		{
			m_state->getPlayer()->m_pos_y += offset;
			if (block.getName(i) == "gemGold.png")
			{
				// Collision with the coin block
				m_state->getPlayer()->m_vy = 0.0f;

				graphics::playSound(m_state->getFullAssetPath("gainingGem.wav"), 1.0f);

				//Increase player's score by 100
				m_state->getPlayer()->addScore(100);
				std::cout << "SCORE OF PLAYER" << m_state->getPlayer()->getScore() << std::endl;
				float posx = block.getBox(i).m_pos_x;
				float posy = block.getBox(i).m_pos_y;
				block.erase(i);

				notifications.buildBlock(posx, posy, 1.0f, 1.0f, "point100.png", graphics::getGlobalTime());



			}
		}
	}

	//Collision with an enemy 
	for (auto enemy : m_enemies) {
		float offset = 0.0f;
		Box ebox = enemy->getBox(1);
		if (offset = m_state->getPlayer()->intersectSideways(ebox))
		{
			int playerPosition = 1;
			// Check the direction of the collision
			// Player collides from the left side of the enemy
			if (m_state->getPlayer()->m_pos_x < ebox.m_pos_x)
				playerPosition = -1;
			m_state->getPlayer()->m_pos_x = ebox.m_pos_x + playerPosition * ebox.m_pos_x; // Adjust player's position
			m_state->getPlayer()->m_vx = -1 * m_state->getPlayer()->m_vx; // Reverse player's horizontal velocity

			graphics::playSound(m_state->getFullAssetPath("collide.wav"), 1.0f);

			// Set vertical velocity to 0
			m_state->getPlayer()->m_vy = 0.0f;
			//When player collides with enemy he loses a heart
			m_state->getPlayer()->loseHeart();
			notifications.buildBlock(ebox.m_pos_x, ebox.m_pos_y, 0.2f, 0.2f, "broken-heart.png", graphics::getGlobalTime());


			break;
		}
	}

	// Check if the player has fallen off the level (below a certain y threshold)
	if ((m_state->getPlayer()->m_pos_y > 4.0f && (m_state->getPlayer()->m_pos_x < 0.0f && m_state->getPlayer()->m_pos_x > -1.0f)) || (m_state->getPlayer()->m_pos_y > 4.0f && (m_state->getPlayer()->m_pos_x < 9.0f && m_state->getPlayer()->m_pos_x > 8.0f)))
	{
		//making the players position to get in from the upper opening 
		m_state->getPlayer()->m_pos_x = -1.0f;
		m_state->getPlayer()->m_pos_y = -10.0f;
	}
}


void Level::update(float dt)
{
	if (m_state->getPlayer()->isActive())
		m_state->getPlayer()->update(dt);

	for (auto& enemy : m_enemies) {
		enemy->update(dt);
	}
	updateMessage(dt);
	checkCollisions();
	if (m_state->getPlayer()->getHearts() == 0) {
		//endGame(); // Call the function to handle the end of the game
		m_state->m_global_offset_x = 0.0f;
		m_state->m_global_offset_y = 0.0f;
		return;
	}


	GameObject::update(dt);

}


void Level::draw()
{
	float w = m_state->getCanvasWidth();
	float h = m_state->getCanvasHeight();

	float offset_x = m_state->m_global_offset_x / 2.0f + w / 2.0f;
	float offset_y = m_state->m_global_offset_y / 2.0f + h / 2.0f;

	//draw background
	graphics::drawRect(offset_x, offset_y, 2.0f * w, 4.0f * w, m_brush_background);

	// draw blocks
	for (int i = 0; i < block.getBlocks().size(); i++)
	{
		drawBlock(i);

	}
	// Draw notifications
	for (int i = 0; i < notifications.getBlocks().size(); i++) {
		drawBlock(i, "N");
	}
	// draw player
	if (m_state->getPlayer()->isActive())
		m_state->getPlayer()->draw();

	//draw enemies
	for (auto& enemy : m_enemies) {
		enemy->draw();
	}

	//draw score

	std::string scoreText = "Score: " + std::to_string(m_state->getPlayer()->getScore());
	graphics::drawText(w / 4, h / 2 + 9, 0.7f, scoreText.c_str(), m_score_brush);


	//draw lives

	graphics::Brush m_heart_brush;
	m_heart_brush.texture = m_state->getFullAssetPath("lives.png");

	int remainingHearts = m_state->getPlayer()->getHearts();

	for (int i = 0; i < remainingHearts; i++)
	{
		// Draw a heart shape at the calculated position
		m_heart_brush.outline_opacity = 0.0f;
		graphics::drawRect(w / 6 + 2 + i, h / 2 + 7.5, 0.7f, 0.7f, m_heart_brush);
	}



}


void Level::init()
{
	// Stage 1
	for (auto p_gob : m_static_objects)
		if (p_gob) p_gob->init();

	for (auto p_gob : m_dynamic_objects)
		if (p_gob) p_gob->init();


	// initialize some collidable blocks here.
	// normally, you should build a Block class 
	// and automate the process more.

	//FRAME
	for (int i = 1; i < 8; i++) {
		block.buildBlock(i, 5);
	}
	block.buildBlock(10, 5);
	block.buildBlock(-2, 5);
	block.buildBlock(-3, 5);
	for (int j = 5; j > -10; j--) {
		block.buildBlock(-3, j);
	}
	block.buildBlock(-2, -9);
	for (int j = 5; j > -9; j--) {
		block.buildBlock(10, j);
	}
	for (int k = 10; k > 1; k--) {
		block.buildBlock(k, -9);
	}

	//
	for (int i = 0; i < 3; i++) {
		block.buildBlock(i, 2);
		block.buildBlock(i, -5);
	}
	for (int i = 4; i < 8; i++) {
		block.buildBlock(i, -1);
	}

	//initializing the gem blocks
	block.buildBlock(0, 0, 1.0F, 1.0F, "gemGold.png");
	block.buildBlock(5, -3, 1.0F, 1.0F, "gemGold.png");
	block.buildBlock(8, 2, 1.0F, 1.0F, "gemGold.png");
	block.buildBlock(-1, -8, 1.0F, 1.0F, "gemGold.png");
	block.buildBlock(8, -7, 1.0F, 1.0F, "gemGold.png");

	m_block_brush.outline_opacity = 0.0f;
	m_block_brush_debug.fill_opacity = 0.1f;
	SETCOLOR(m_block_brush_debug.fill_color, 0.1f, 1.0f, 0.1f);
	SETCOLOR(m_block_brush_debug.outline_color, 0.3f, 1.0f, 0.2f);

	Enemy* enemy1 = new Enemy("enemy1");
	enemy1->init();
	m_enemies.push_back(enemy1);

	Enemy* enemy2 = new Enemy("enemy2");
	enemy2->init(5, -6);
	m_enemies.push_back(enemy2);

	Enemy* enemy3 = new Enemy("enemy3");
	enemy3->init(8, -4);
	m_enemies.push_back(enemy3);

	Enemy* enemy4 = new Enemy("enemy4");
	enemy4->init(3, -3);
	m_enemies.push_back(enemy4);

	Enemy* enemy5 = new Enemy("enemy5");
	enemy5->init(-3, -7);
	m_enemies.push_back(enemy5);

	Enemy* enemy6 = new Enemy("enemy6");
	enemy6->init(-1, 1);
	m_enemies.push_back(enemy6);

	Enemy* enemy7 = new Enemy("enemy7");
	enemy7->init(4, -1);
	m_enemies.push_back(enemy7);

	Enemy* enemy8 = new Enemy("enemy8");
	enemy8->init(-2, -7);
	m_enemies.push_back(enemy8);
}


Level::Level(const std::string& name)
	: GameObject(name)
{

	m_brush_background.outline_opacity = 0.0f;
	m_brush_background.fill_opacity = 1.0f;
	SETCOLOR(m_brush_background.fill_color, 0.0f, 0.0f, 0.0f);



}

Level::~Level()
{
	for (auto p_go : m_static_objects)
		delete p_go;
	for (auto p_go : m_dynamic_objects)
		delete p_go;
	for (auto enemies : m_enemies) {
		delete enemies;
	}
}
