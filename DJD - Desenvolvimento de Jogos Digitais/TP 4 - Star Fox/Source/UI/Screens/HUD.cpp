//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "PauseMenu.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{

    Vector2 healthPos = Vector2(-250.0f, 340.0f); 
    Vector2 scoreLabelPos = Vector2(-350.0f, 295.0f); 
    Vector2 scoreValuePos = Vector2(-260.0f, 295.0f);

    // Health segments (added in order of drawing for correct layering)
    mHealth3 = AddImage("../Assets/HUD/ShieldBlue.png", healthPos, 0.75f);
    mHealth2 = AddImage("../Assets/HUD/ShieldOrange.png", healthPos, 0.75f);
    mHealth1 = AddImage("../Assets/HUD/ShieldRed.png", healthPos, 0.75f);

    // Health bar background
    AddImage("../Assets/HUD/ShieldBar.png", healthPos, 0.75f);

    // Score text
    AddText("Score: ", scoreLabelPos, 0.75f);
    mScore = AddText("0", scoreValuePos, 0.75f);
}

void HUD::SetHealth(int health)
{
    mHealth1->SetIsVisible(health >= 1);
    mHealth2->SetIsVisible(health >= 2);
    mHealth3->SetIsVisible(health >= 3);
}

void HUD::SetScore(int score)
{
    mScore->SetText(std::to_string(score));
}

void HUD::HandleKeyPress(int key)
{
    UIScreen::HandleKeyPress(key);

    // If Enter is pressed and the game is not paused
    if (key == SDLK_RETURN && !mGame->IsPaused()) {
        mGame->SetPaused(true);
        new PauseMenu(mGame, "../Assets/Fonts/Arial.ttf");
    }
}