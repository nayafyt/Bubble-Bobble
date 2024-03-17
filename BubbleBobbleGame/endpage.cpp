#include "endpage.h"
#include "util.h"

void EndPage::drawEP() {
    float w = m_state->getCanvasWidth();
    float h = m_state->getCanvasHeight();

    float offset_x = m_state->m_global_offset_x / 2.0f + w / 2.0f;
    float offset_y = m_state->m_global_offset_y / 2.0f + h / 2.0f;

    graphics::drawText(w / 4.0f, h / 5.0f, 1.5f, "Game Over", m_brush_text);

    graphics::setFont("assets\\PressStart2P-vaV7.ttf");
    int playerScore = GameState::getInstance()->getPlayerScore();

    // Display the player's score
    char scoreText[20];
    sprintf_s(scoreText, "Score: %d", playerScore);
    //graphics::drawText(w / 5.0f - 2, h / 2.0f, 0.8f, scoreText, m_brush_text);
    if (playerScore == 500)
    {
        graphics::drawText(w / 5.0f - 2, h / 2.0f, 0.8, "YOU WON!!!", m_brush_text);
    }
    else {
        graphics::drawText(w / 5.0f - 2, h / 2.0f,0.8, "You died:(", m_brush_text);

    }

}

void EndPage::initEP() {
    m_brush_background.outline_opacity = 0.0f;
    m_brush_text.fill_color[0] = 1.0f;
    m_brush_text.fill_color[1] = 1.0f;
    m_brush_text.fill_color[2] = 1.0f;
    graphics::playMusic("assets\\game_over.mp3", 0.11f, true);
    m_brush_title.outline_opacity = 0.0f;

}