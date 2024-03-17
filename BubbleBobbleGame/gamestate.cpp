#include "gamestate.h"
#include "level.h"
#include "player.h"
#include "startpage.h"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std::chrono_literals;

GameState::GameState()
{
}

GameState::~GameState()
{
	if (m_current_level)
		delete m_current_level;
}

GameState* GameState::getInstance()
{
	if (!m_unique_instance)
	{
		m_unique_instance = new GameState();
	}
	return m_unique_instance;
}

bool GameState::init()
{
	m_current_level = new Level("1.lvl");
	m_current_level->init();

	m_player = new Player("Player");
	m_player->init();

	graphics::preloadBitmaps(getAssetDir());
	graphics::setFont(m_asset_path + "OpenSans-Regular.ttf");

	return true;
}

bool GameState::starting() {

	inStartPage = true;

	std::cout << "The name is Level0." << std::endl;
	if (graphics::getKeyState(graphics::SCANCODE_RETURN)) {
		std::cout << "you press enter" << std::endl;
	}
	return true;
}

bool GameState::isInStartPage() const {
	return inStartPage;
}

bool GameState::isInEndPage()const {
	return inEndPage;
}
void GameState::draw()
{
	if (inStartPage) {
		if (!m_starting)
			return;

		m_starting->draw();
	}
	else {

		if (!m_current_level)
			return;

		m_current_level->draw();
	}

}

void GameState::update(float dt)
{

	if (!inStartPage) {
		// Skip an update if a long delay is detected 
		// to avoid messing up the collision simulation
		if (dt > 500) // ms
			return;

		// Avoid too quick updates
		float sleep_time = std::max(17.0f - dt, 0.0f);
		if (sleep_time > 0.0f)
		{
			std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(sleep_time));
		}

		if (!m_current_level)
			return;

		m_current_level->update(dt);

		m_debugging = graphics::getKeyState(graphics::SCANCODE_0);
	}

}

int GameState::getPlayerScore() const {
	if (m_player) {
		return m_player->getScore();
	}
	return 0;
}

void GameState::setInEndPage(bool inEndPage) {
	this->inEndPage = inEndPage;
}

bool GameState::isGameOver() const {
	return(m_player->getScore() == 500 || m_player->getHearts() == 0);
}

std::string GameState::getFullAssetPath(const std::string& asset)
{
	return m_asset_path + asset;
}

std::string GameState::getAssetDir()
{
	return m_asset_path;
}

GameState* GameState::m_unique_instance = nullptr;