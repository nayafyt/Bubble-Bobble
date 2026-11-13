#include "gamestate.h"
#include "level.h"
#include "player.h"
#include "startpage.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#ifdef _WIN32
// windows.h defines min/max as macros, which breaks the std::max call in
// GameState::update below, and pulls in a great deal we do not need.
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <climits>
#else
#include <unistd.h>
#include <climits>
#endif

using namespace std::chrono_literals;

namespace
{
	/** Directory holding the running executable, with a trailing separator.
	*   Returns an empty string if it cannot be determined.
	*/
	std::string executableDir()
	{
		std::string path;
#ifdef _WIN32
		char buffer[MAX_PATH];
		DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		if (length == 0)
			return "";
		path.assign(buffer, length);
#elif defined(__APPLE__)
		char buffer[PATH_MAX];
		uint32_t size = sizeof(buffer);
		if (_NSGetExecutablePath(buffer, &size) != 0)
			return "";
		path = buffer;
#else
		char buffer[PATH_MAX];
		ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
		if (length <= 0)
			return "";
		path.assign(buffer, length);
#endif
		const size_t slash = path.find_last_of("/\\");
		return slash == std::string::npos ? "" : path.substr(0, slash + 1);
	}

	/** Locates the assets directory.
	*
	*   The binary is built into build/ and the assets live in assets/ beside
	*   it, but the Visual Studio build and a plain `cd build && ./BubbleBobble`
	*   put the two in different relative positions. Probe the likely spots for
	*   a file we know must be there rather than depending on which directory
	*   the game happened to be started from.
	*/
	std::string findAssetDir()
	{
		const std::string exe = executableDir();
		const std::string candidates[] = {
			exe + "../assets/",
			exe + "assets/",
			"assets/",
			"../assets/",
		};

		for (const std::string& dir : candidates)
		{
			std::ifstream probe(dir + "tiles.png");
			if (probe.good())
				return dir;
		}

		// Nothing found: fall back and let the asset loader report what is missing.
		std::cout << "Warning: could not locate the assets directory." << std::endl;
		return "assets/";
	}
}

GameState::GameState()
	: m_asset_path(findAssetDir())
{
}

GameState::~GameState()
{
	delete m_current_level;
	delete m_player;
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
	// init() also runs on a restart, so throw away the previous run first
	// instead of leaking it.
	delete m_current_level;
	m_current_level = nullptr;
	delete m_player;
	m_player = nullptr;

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
	if (!m_player)
		return false;
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