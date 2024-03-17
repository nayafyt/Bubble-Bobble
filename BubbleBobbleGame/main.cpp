#include <sgg/graphics.h>
#include "startpage.h"
#include "endpage.h"
#include <iostream>
#include "gamestate.h"

StartPage startPage;
EndPage endPage;
GameState* gameState = GameState::getInstance();

void draw() {
    if (gameState->isInStartPage()) {
        startPage.drawSP();
    }
    else if (gameState->isInEndPage())  // Check if in EndPage
    {
        endPage.drawEP();
    }
    else {
        gameState->draw();
    }
}

void update(float dt) {
    if (gameState->isInStartPage()) {
        //If enter key is pressed you start the game 
        if (graphics::getKeyState(graphics::SCANCODE_RETURN)) {
            gameState->setInStartPage(false); //Mark StartPage as shown. 
            gameState->init(); // Start game on level1 
            graphics::stopMusic();
        }
    }
    else if (gameState->isInEndPage())  // Check if in EndPage
    {
        if (graphics::getKeyState(graphics::SCANCODE_R))
        {

            gameState->setInEndPage(false);
            gameState->starting();
            startPage.initSP();
        }
    }
    else
    {
        gameState->update(dt);

        if (gameState->isGameOver()) {
            gameState->setInEndPage(true);
        }
    }

}


int main(int argc, char** argv)
{
    graphics::createWindow(800, 800, "Bubble Bobble");

    gameState->starting();
    startPage.initSP();

    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);

    graphics::setCanvasSize(GameState::getInstance()->getCanvasWidth(), GameState::getInstance()->getCanvasHeight());
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    graphics::setFont("assets\\PressStart2P-vaV7.ttf");


    graphics::startMessageLoop();
    return 0;
}

