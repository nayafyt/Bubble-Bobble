
#pragma once

#include <string>

class GameState
{
private:
	static GameState* m_unique_instance;

	// Resolved once at startup, relative to the executable, so the game can
	// be launched from any working directory.
	std::string m_asset_path;

	const float m_canvas_width = 20.0f;
	const float m_canvas_height = 20.0f;

	class Level* m_current_level = 0;
	class StartPage* m_starting = 0;
	class Player* m_player = 0;

	//Bool var to check if the game is in the start page
	bool inStartPage = true;
	bool inEndPage = false;
	GameState();

public:
	float m_global_offset_x = 0.0f;
	float m_global_offset_y = 0.0f;

	bool m_debugging = false;

public:
	~GameState();
	void resetGlobalOffsets();
	static GameState* getInstance();

	bool starting();
	bool ending();
	bool init();
	void draw();
	void update(float dt);

	int getPlayerScore() const;

	void setInEndPage(bool inEndPage);

	bool isGameOver() const;

	std::string getFullAssetPath(const std::string& asset);
	std::string getAssetDir();

	float getCanvasWidth() { return m_canvas_width; }
	float getCanvasHeight() { return m_canvas_height; }

	class Player* getPlayer() { return m_player; }

	//Check if the game is in the start page
	bool isInStartPage() const;

	//Check if the game is in the end page
	bool isInEndPage() const;

	//Change start page state
	void setInStartPage(bool a) { inStartPage = a; }


};
